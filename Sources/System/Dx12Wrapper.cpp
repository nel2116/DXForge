// [Dx12Wrapper.cpp]
// �쐬�� : 2024/05/23 15:00
// �쐬�� : �c���~�m��
// �T�v   : DirectX 12���b�p�[�N���X�̎���
#include "stdafx.h"
#include "Dx12Wrapper.h"
#include <cassert>
#include <DirectX/d3dx12.h>
#include <Application/Application.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

namespace
{
	/// <summary>
	/// ���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
	/// @param modelPath : Application���猩��PMD���f���̃p�X
	/// @param texPath : PMD���f�����猩���e�N�X�`���̃p�X
	/// @return : Application���猩���e�N�X�`���̃p�X
	/// </summary>
	string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath)
	{
		// �t�@�C���̃t�H���_��؂��\��/�̂Ɏ�ނ��g�p�����\��������
		// �Ƃ�����������\��/�𓾂���΂����̂ŁA�o����rfind�����A��r����
		// int�^�������Ă���̂͌�����Ȃ������ꍇ��rfind��epos(-1��0xffffffff)��Ԃ�����
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	/// <summary>
	/// �t�@�C��������g���q���擾����
	/// @param filePath : �t�@�C���p�X
	/// @return : �t�@�C���̊g���q
	/// </summary>
	string GetExtension(const string& filePath)
	{
		int idx = filePath.rfind('.');
		return filePath.substr(idx + 1, filePath.length() - idx - 1);
	}

	/// <summary>
	/// �t�@�C��������g���q���擾����(���C�h������)
	/// @param filePath : �t�@�C���p�X
	/// @return : �t�@�C���̊g���q
	/// </summary>
	wstring GetExtension(const wstring& filePath)
	{
		int idx = filePath.rfind(L'.');
		return filePath.substr(idx + 1, filePath.length() - idx - 1);
	}

	/// <summary>
	/// �e�N�X�`���̃p�X���Z�p���[�^�����ŕ�������
	/// @param texPath : �e�N�X�`���̃p�X
	/// @param separator : ��؂蕶��
	/// @return : �����O��̕�����y�A
	/// </summary>
	pair<string, string> SplitFileName(const string& texPath, const char separator = '*')
	{
		int idx = texPath.find(separator);
		pair<string, string> ret;
		ret.first = texPath.substr(0, idx);
		ret.second = texPath.substr(idx + 1, texPath.length() - idx - 1);
		return ret;
	}

	/// <summary>
	/// string(�}���`�o�C�g����)����wstring(���C�h����)�ɕϊ�����
	/// @param str : �ϊ����̕�����
	/// @return : �ϊ���̕�����
	/// </summary>
	wstring GetWideStringFromString(const string& str)
	{
		// �Ăяo������(�����񐔂𓾂�)
		int num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

		wstring wstr;	// string��wchar_t��
		wstr.resize(num1);	// wchar_t�ł�string�����T�C�Y

		// �Ăяo������(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
		auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

		assert(num1 == num2);	// ���ڂƓ��ڂŕ����񐔂��Ⴄ�̂͂�������
		return wstr;
	}

	/// <summary>
	/// �f�o�b�O���C���[��L���ɂ���
	/// </summary>
	void EnableDebugLayer()
	{
		ComPtr<ID3D12Debug> debugLayer = nullptr;
		auto res = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
		debugLayer->EnableDebugLayer();
	}
}

Dx12Wrapper::Dx12Wrapper(HWND hwnd)
{
#ifdef _DEBUG
	// �f�o�b�O���C���[��L���ɂ���
	EnableDebugLayer();
#endif // _DEBUG

	auto& app = Application::Instance();
	m_winSize = app.GetWindowSize();

	// DirectX12�֘A�̏�����
	if (FAILED(InitDXGIDevice())) { assert(0); return; }
	if (FAILED(InitCommand())) { assert(0); return; }
	if (FAILED(CreateSwapChain(hwnd))) { assert(0); return; }
	if (FAILED(CreateFinalRenderTargets())) { assert(0); return; }
	if (FAILED(CreateSceneView())) { assert(0); return; }

	// �e�N�X�`�����[�_�[�֘A�̏�����
	CreateTextureLoaderTable();

	// �[�x�o�b�t�@�̐���
	if (FAILED(CreateDepthStencilView())) { assert(0); return; }

	// �t�F���X�̍쐬
	if (FAILED(m_device->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()))))
	{
		assert(0);
		return;
	}
}

Dx12Wrapper::~Dx12Wrapper()
{

}

void Dx12Wrapper::Update()
{
}

void Dx12Wrapper::BeginDraw()
{
	// Direct3D�̕`�揈���J�n
	// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_cmdList->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g�̎w��
	auto rtvH = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// �[�x���w��
	auto dsvH = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	m_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ��ʃN���A
	float clearColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };	// ���F
	m_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	m_cmdList->RSSetViewports(1, m_viewport.get());
	m_cmdList->RSSetScissorRects(1, m_scissorRect.get());
}

void Dx12Wrapper::EndDraw()
{
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_cmdList->ResourceBarrier(1, &barrier);

	// �R�}���h�̃N���[�Y
	m_cmdList->Close();

	// �R�}���h�̎��s
	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmndQueue->ExecuteCommandLists(1, cmdLists);
	//// �҂�
	m_cmndQueue->Signal(m_fence.Get(), ++m_fenceVal);

	if (m_fence->GetCompletedValue() < m_fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	m_cmdAllocator->Reset(); // �L���[�����Z�b�g
	m_cmdList->Reset(m_cmdAllocator.Get(), nullptr); // �R�}���h���X�g�����Z�b�g
}

ComPtr<ID3D12Resource> Dx12Wrapper::GetTextureByPath(const char* texPath)
{
	auto it = m_textureTable.find(texPath);
	if (it != m_textureTable.end())
	{
		// �e�[�u�����ɂ������烍�[�h����̂ł͂Ȃ��}�b�v���̃��\�[�X��Ԃ�
		return m_textureTable[texPath];
	}
	else
	{
		return ComPtr<ID3D12Resource>(CreateTextureFromFile(texPath));
	}
}

ComPtr<ID3D12Device> Dx12Wrapper::GetDevice()
{
	return m_device;
}

ComPtr<ID3D12GraphicsCommandList> Dx12Wrapper::GetCommandList()
{
	return m_cmdList;
}

ComPtr<IDXGISwapChain4> Dx12Wrapper::GetSwapChain()
{
	return m_swapChain;
}

void Dx12Wrapper::SetScene()
{
	// ���݂̃V�[��(�r���[�v���W�F�N�V����)���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { m_sceneDescHeap.Get() };
	m_cmdList->SetDescriptorHeaps(1, heaps);
	m_cmdList->SetGraphicsRootDescriptorTable(0, m_sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

HRESULT Dx12Wrapper::CreateFinalRenderTargets()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto hr = m_swapChain->GetDesc1(&desc);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "[CreateFinalRenderTargets]�X���b�v�`�F�C���̎擾�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// �����_�[�^�[�Q�b�g�r���[�p
	heapDesc.NodeMask = 0;	// �}���`GPU�̏ꍇ�Ɏg�p
	heapDesc.NumDescriptors = 2;	// �����_�[�^�[�Q�b�g�r���[��2��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �t���O�͓��ɂȂ�

	hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_rtvHeaps.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "[CreateFinalRenderTargets]�����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̐����Ɏ��s���܂���", "�G���[", MB_OK);
		SUCCEEDED(hr);
		return hr;
	}

	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = m_swapChain->GetDesc(&swcDesc);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "[CreateFinalRenderTargets]�X���b�v�`�F�C���̎擾�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}
	m_backBuffers.resize(swcDesc.BufferCount);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	// SRGB�t�H�[�}�b�g�̃����_�[�^�[�Q�b�g�r���[���쐬
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// SRGB�t�H�[�}�b�g
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// �e�N�X�`���Ƃ��Ĉ���

	for (int i = 0; i < swcDesc.BufferCount; ++i)
	{
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
		assert(SUCCEEDED(hr));
		rtvDesc.Format = m_backBuffers[i]->GetDesc().Format;
		m_device->CreateRenderTargetView(m_backBuffers[i], &rtvDesc, handle);
		handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	m_viewport.reset(new CD3DX12_VIEWPORT(m_backBuffers[0]));
	m_scissorRect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));
	return hr;
}

HRESULT Dx12Wrapper::CreateDepthStencilView()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto hr = m_swapChain->GetDesc1(&desc);

	// �[�x�o�b�t�@�̍쐬
	// �[�x�o�b�t�@�̎g�p
	// auto depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, 
	//	desc.Width, desc.Height, 
	// 1, 0, 1, 0, 
	// D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resdesc.DepthOrArraySize = 1;
	resdesc.Width = desc.Width;
	resdesc.Height = desc.Height;
	resdesc.Format = DXGI_FORMAT_D32_FLOAT;
	resdesc.SampleDesc.Count = 1;
	resdesc.SampleDesc.Quality = 0;
	resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resdesc.MipLevels = 1;
	resdesc.Alignment = 0;


	// �f�v�X�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	hr = m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �f�v�X�������݂Ɏg�p
		&depthClearValue,
		IID_PPV_ARGS(m_depthBuffer.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return hr; }

	// �[�x�̂��߂̃f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};	// �[�x�Ɏg����Ƃ������Ƃ��킩��Ηǂ�
	dsvHeapDesc.NumDescriptors = 1;	// �[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	// �[�x�r���[�p
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �t���O�͓��ɂȂ�
	hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return hr; }

	// �[�x�r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // �f�v�X�l��32bit�g�p
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // �e�N�X�`���Ƃ��Ĉ���
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // �t���O�͓��ɂȂ�
	m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// ���Ȃ��쐬�ł���
	return S_OK;
}

// �X���b�v�`�F�C���̐���
HRESULT Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	RECT rc = {};
	::GetWindowRect(hwnd, &rc);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_winSize.cx;
	swapChainDesc.Height = m_winSize.cy;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto hr = m_dxgiFactory->CreateSwapChainForHwnd(m_cmndQueue.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)m_swapChain.ReleaseAndGetAddressOf());

	// �G���[�`�F�b�N
	assert(SUCCEEDED(hr));

	return hr;
}

HRESULT Dx12Wrapper::InitDXGIDevice()
{
	UINT flagsDXGI = 0;
#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	auto hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf()));

	// �G���[�`�F�b�N
	if (FAILED(hr)) { return hr; }

	// DirectX12�֘A�̏�����
	// �t�B�`���[���x����
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	// �f�o�C�X�̐���
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	// �A�_�v�^�[�̗�
	for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	hr = S_FALSE;
	// Direct3D�f�o�C�X�̏�����
	D3D_FEATURE_LEVEL featureLevel;
	for (auto l : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()))))
		{
			featureLevel = l;
			hr = S_OK;
			break;
		}
	}

	return hr;
}

HRESULT Dx12Wrapper::InitCommand()
{
	// �R�}���h�L���[�̐���
	auto hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { assert(0); return hr; }

	// �R�}���h���X�g�̐���
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_cmdList.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { assert(0); return hr; }


	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.NodeMask = 0;	// �}���`GPU�̏ꍇ�Ɏg�p
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �v���C�I���e�B���Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// �R�}���h���X�g�ƍ��킹��
	hr = m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_cmndQueue.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));
	return hr;
}

// �r���[�v���W�F�N�V�����p�r���[�̐���
HRESULT Dx12Wrapper::CreateSceneView()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto hr = m_swapChain->GetDesc1(&desc);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "�X���b�v�`�F�C���̎擾�Ɏ��s���܂���", "�G���[", MB_OK);
		return hr;
	}

	// �萔�o�b�t�@�쐬
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);	// �A�b�v���[�h�p�q�[�v
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);	// �V�[���f�[�^�̃T�C�Y
	hr = m_device->CreateCommittedResource(
		&heapProp,	// �A�b�v���[�h�p�q�[�v
		D3D12_HEAP_FLAG_NONE,	// ���Ƀt���O�͂Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// ���\�[�X�̏��
		nullptr,	// �N���A�l
		IID_PPV_ARGS(m_sceneConstBuff.ReleaseAndGetAddressOf()));

	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "�V�[���萔�o�b�t�@�̐����Ɏ��s���܂���", "�G���[", MB_OK);
		assert(SUCCEEDED(hr));
		return hr;
	}

	// �}�b�v
	m_mappedSceneData = nullptr;	// �}�b�v��������|�C���^�[
	hr = m_sceneConstBuff->Map(0, nullptr, (void**)&m_mappedSceneData);	// �}�b�v
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "�V�[���萔�o�b�t�@�̃}�b�v�Ɏ��s���܂���", "�G���[", MB_OK);
		assert(SUCCEEDED(hr));
		return hr;
	}

	// �r���[�s��A�v���W�F�N�V�����s��̐���
	XMFLOAT3 eye(0.0f, 15.0f, -15.0f);	// ���_
	XMFLOAT3 target(0.0f, 15.0f, 0.0f);	// �����_
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);		// �����
	m_mappedSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));	// �r���[�s��
	m_mappedSceneData->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,	// ��p��45��
		static_cast<float>(desc.Width) / static_cast<float>(desc.Height),	// �A�X�y�N�g��
		0.1f,
		1000.0f);	// �v���W�F�N�V�����s��
	m_mappedSceneData->eye = eye;

	// �f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �V�F�[�_���猩����悤��
	descHeapDesc.NodeMask = 0;	// �}���`GPU�̏ꍇ�Ɏg�p
	descHeapDesc.NumDescriptors = 1;	// �f�X�N���v�^��1��
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// CBV,SRV,UAV�p
	hr = m_device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(m_sceneDescHeap.ReleaseAndGetAddressOf()));

	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "�V�[���f�X�N���v�^�q�[�v�̐����Ɏ��s���܂���", "�G���[", MB_OK);
		assert(SUCCEEDED(hr));
		return hr;
	}

	// �f�X�N���v�^�q�[�v�̐擪�A�h���X���擾
	auto heapHandle = m_sceneDescHeap->GetCPUDescriptorHandleForHeapStart();
	// �f�X�N���v�^�T�C�Y���擾
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_sceneConstBuff->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	cbvDesc.SizeInBytes = m_sceneConstBuff->GetDesc().Width;	// �T�C�Y��16�o�C�g�A���C�����g

	// �萔�o�b�t�@�r���[�̍쐬
	m_device->CreateConstantBufferView(&cbvDesc, heapHandle);
	return hr;
}

// �e�N�X�`�����[�_�e�[�u���̍쐬
void Dx12Wrapper::CreateTextureLoaderTable()
{
	m_loadLambdaTable["sph"] = m_loadLambdaTable["spa"] = m_loadLambdaTable["bmp"] = m_loadLambdaTable["png"] = m_loadLambdaTable["jpg"] = [](const wstring& path, TexMetadata* metadata, ScratchImage& img)->HRESULT
	{
		return LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, metadata, img);
	};
	m_loadLambdaTable["tga"] = [](const wstring& path, TexMetadata* metadata, ScratchImage& img)->HRESULT
	{
		return LoadFromTGAFile(path.c_str(), metadata, img);
	};
	m_loadLambdaTable["dds"] = [](const wstring& path, TexMetadata* metadata, ScratchImage& img)->HRESULT
	{
		return LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, metadata, img);
	};
}

// �e�N�X�`��������e�N�X�`���o�b�t�@�쐬�A���g���R�s�[
ID3D12Resource* Dx12Wrapper::CreateTextureFromFile(const char* texPath)
{
	string texPathStr = texPath;
	// �e�N�X�`���̃��[�h
	TexMetadata metadata = {};
	ScratchImage ScratchImg = {};
	auto wTexPath = GetWideStringFromString(texPathStr);	// �e�N�X�`���̃t�@�C���p�X
	auto ext = GetExtension(texPathStr);	// �t�@�C���̊g���q���擾
	auto hr = m_loadLambdaTable[ext](wTexPath, &metadata, ScratchImg);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		// ���b�Z�[�W�{�b�N�X�ŃG���[��\��
		MessageBox(nullptr, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", "�G���[", MB_OK);
		return nullptr;
	}

	auto img = ScratchImg.GetImage(0, 0, 0);	// ���f�[�^���o

	// WriteToSubresource�œ]������p�̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

	ID3D12Resource* texBuff = nullptr;
	hr = m_device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���Ƀt���O�͂Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "�e�N�X�`���o�b�t�@�̐����Ɏ��s���܂���", "�G���[", MB_OK);
		return nullptr;
	}

	hr = texBuff->WriteToSubresource(0, nullptr, img->pixels, img->rowPitch, img->slicePitch);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "�e�N�X�`���o�b�t�@�ւ̏������݂Ɏ��s���܂���", "�G���[", MB_OK);
		return nullptr;
	}

	// ��肪�Ȃ����
	return texBuff;
}
