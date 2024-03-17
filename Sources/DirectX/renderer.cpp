#include <Precompils/stdafx.h>
#include "renderer.h"

Renderer::Renderer()
	: m_dev(nullptr)
	, m_dxgiFactory(nullptr)
	, m_swapChain(nullptr)
	, m_cmdAllocator(nullptr)
	, m_cmdList(nullptr)
	, m_cmdQueue(nullptr)
	, m_pRTV(nullptr)
	, m_fence(nullptr)
	, m_fenceVal(0)
{
}

Renderer::~Renderer()
{
	// �������


	// �o�b�N�o�b�t�@�̉��
	for (auto bb : m_backBuffers)
	{
		SAFE_RELEASE(bb);
	}
	// �t�F���X�̉��
	SAFE_RELEASE(m_fence);
	// �����_�[�^�[�Q�b�g�r���[�̉��
	SAFE_RELEASE(m_pRTV);
	// �R�}���h�L���[�̉��
	SAFE_RELEASE(m_cmdQueue);
	// �R�}���h�A���P�[�^�̉��
	SAFE_RELEASE(m_cmdAllocator);
	// �R�}���h���X�g�̉��
	SAFE_RELEASE(m_cmdList);
	// �X���b�v�`�F�C���̉��
	SAFE_RELEASE(m_swapChain);
	// DXGI�t�@�N�g���̉��
	SAFE_RELEASE(m_dxgiFactory);
	// �f�o�C�X�̉��
	SAFE_RELEASE(m_dev);
}

bool Renderer::Init()
{
#ifdef _DEBUG

	// �f�o�b�O���C���[�̗L����
	// �f�o�C�X�����O�ɂ���Ă����Ȃ��ƁA�f�o�C�X�����X�g���Ă��܂�
	EnableDebugLayer();

#endif // _DEBUG

	// ----- DirectX12�̏����� -----

	// �f�o�C�X�̍쐬
	if (!CreateDevice()) { return false; }
	// �R�}���h���X�g�̍쐬
	if (!CreateCommandList()) { return false; }
	// �X���b�v�`�F�C���̍쐬
	if (!CreateSwapChain()) { return false; }
	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	if (!CreateRenderTargetView()) { return false; }
	// �t�F���X�̍쐬
	if (!CreateFence()) { return false; }

	// ����ɏ������ł���
	return true;
}

void Renderer::Draw()
{
	// �o�b�N�o�b�t�@�̃C���f�b�N�X
	UINT bbIdx = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	                    // ���\�[�X�̏�ԑJ��
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		                    // ���Ɏw��Ȃ�
	barrier.Transition.pResource = m_backBuffers[bbIdx];	                    // ���\�[�X
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// �T�u���\�[�X
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	            // 
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	        //
	m_cmdList->ResourceBarrier(1, &barrier);	                                // ���\�[�X�o���A�̐ݒ�

	// �����_�[�^�[�Q�b�g���w��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * bbIdx;
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// ��ʃN���A
	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };	// ���F
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// �����_�[�^�[�Q�b�g�̏�ԑJ��
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;	        //
	m_cmdList->ResourceBarrier(1, &barrier);

	// �R�}���h���X�g�̃N���[�Y
	m_cmdList->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { m_cmdList };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	// �҂�
	m_cmdQueue->Signal(m_fence, ++m_fenceVal);
	if (m_fence->GetCompletedValue() < m_fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// �L���[���N���A
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator, nullptr);	// �ĂуR�}���h���X�g�����߂鏀��

	// �o�b�t�@�̐؂�ւ�
	m_swapChain->Present(1, 0);
}

void Renderer::EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	if (SUCCEEDED(hr))
	{	// ����Ƀf�o�b�O���C���[���擾�ł���
		debugLayer->EnableDebugLayer();	// �f�o�b�O���C���[��L���ɂ���
		debugLayer->Release();			// �C���^�[�t�F�[�X�����
	}
	else
	{	// �f�o�b�O���C���[���擾�ł��Ȃ�����
		MessageBox(nullptr, "D3D12GetDebugInterface Failed.", "Error", MB_OK);
	}
}

bool Renderer::CreateDevice()
{
	// DXGI�t�@�N�g���̐���
	HRESULT result = S_OK;
	if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory))))
	{
		if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory))))
		{
			MessageBox(nullptr, "CreateDXGIFactory2 Failed.", "Error", MB_OK);
			return false;
		}
	}

	// �A�_�v�^�[�̗�
	IDXGIAdapter1* adapter = nullptr;
	vector<IDXGIAdapter1*> adapters;
	for (int i = 0; m_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(adapter);
	}
	// �A�_�v�^�[�����ʂ��邽�߂̏����擾
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC1 desc;
		adpt->GetDesc1(&desc);
		wstring strDesc = desc.Description;	// �A�_�v�^�[�̐���
		// �T�������A�_�v�^�[�̐������w��
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			adapter = adpt;
			break;
		}
	}

	// �f�o�C�X�̐���
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels)
	{
		if (D3D12CreateDevice(adapter, lv, IID_PPV_ARGS(&m_dev)) == S_OK)
		{
			featureLevel = lv;
			break;
		}
	}

	// �G���[�`�F�b�N
	if (FAILED(result))
	{
		MessageBox(nullptr, "D3D12CreateDevice Failed.", "Error", MB_OK);
		return false;
	}

	// �A�_�v�^�[�̉��
	for (auto adpt : adapters)
	{
		SAFE_RELEASE(adpt);
	}

	return true;
}

bool Renderer::CreateCommandList()
{
	// �R�}���h�A���P�[�^�̐���
	auto hr = m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommandAllocator Failed.", "Error", MB_OK);
		return false;
	}

	// �R�}���h���X�g�̐���
	hr = m_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator, nullptr, IID_PPV_ARGS(&m_cmdList));

	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommandList Failed.", "Error", MB_OK);
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	        // �^�C���A�E�g�Ȃ�
	queueDesc.NodeMask = 0;	                                    // �A�_�v�^�[�͈�����g��Ȃ��Ƃ���0
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	        // �R�}���h���X�g�ƍ��킹��

	// �R�}���h�L���[�̐���
	hr = m_dev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_cmdQueue));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommandQueue Failed.", "Error", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = WINDOW_WIDTH;					            // �E�B���h�E�̕�
	swapChainDesc.Height = WINDOW_HEIGHT;				            // �E�B���h�E�̍���
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	            // �s�N�Z���t�H�[�}�b�g
	swapChainDesc.Stereo = false;						            // �X�e���I�\��
	swapChainDesc.SampleDesc.Count = 1;					            // �}���`�T���v�����O�̐ݒ�
	swapChainDesc.SampleDesc.Quality = 0;				            // �}���`�T���v�����O�̕i��
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	            // �o�b�t�@�̎g�p�@
	swapChainDesc.BufferCount = 2;						            // �o�b�t�@�̐�
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;		            // �o�b�N�o�b�t�@�̓E�B���h�E�T�C�Y�ɍ��킹�Ċg��k��
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	    // �t���b�v��̓o�b�N�o�b�t�@��j��
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;	        // �A���t�@���[�h
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E���[�h�ƃt���X�N���[�����[�h�̐؂�ւ�������

	// �X���b�v�`�F�C���̐���
	auto hr = m_dxgiFactory->CreateSwapChainForHwnd(m_cmdQueue, GetWindowHandle(), &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&m_swapChain);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateSwapChainForHwnd Failed.", "Error", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::CreateRenderTargetView()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	    // �����_�[�^�[�Q�b�g�r���[
	heapDesc.NodeMask = 0;							    // �m�[�h�}�X�N
	heapDesc.NumDescriptors = 2;					    // �f�B�X�N���v�^�̐�, �o�b�t�@�̐��Ɠ���
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// ���Ɏw��Ȃ�

	// �f�B�X�N���v�^�q�[�v�̐���
	auto hr = m_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pRTV));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateDescriptorHeap Failed.", "Error", MB_OK);
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = m_swapChain->GetDesc(&swcDesc);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "GetDesc Failed.", "Error", MB_OK);
		return false;
	}

	m_backBuffers.resize(swcDesc.BufferCount);

	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		hr = m_swapChain->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(&m_backBuffers[idx]));
		if (FAILED(hr))
		{
			MessageBox(nullptr, "GetBuffer Failed.", "Error", MB_OK);
			return false;
		}
		m_dev->CreateRenderTargetView(m_backBuffers[idx], nullptr, handle);
		handle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	return true;
}

bool Renderer::CreateFence()
{
	// �t�F���X�̐���
	auto hr = m_dev->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

	return true;
}
