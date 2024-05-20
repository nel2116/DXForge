#include <stdafx.h>
#include "renderer.h"
#include <d3dx12.h>

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
	, m_vertexBuff(nullptr)
	, m_vertexBuffView()
	, m_indexBuff(nullptr)
	, m_indexBuffView()
	, m_vsBlob(nullptr)
	, m_psBlob(nullptr)
	, m_backBuffers()
	, m_inputLayout{ nullptr }
	, m_rootSignature(nullptr)
	, m_pipelineState(nullptr)
	, m_viewport()
	, m_scissorRect()
	, m_texData()
	, m_texBuff(nullptr)
	, m_pBasicDescHeap(nullptr)
	, m_constBuff(nullptr)
{
}

Renderer::~Renderer()
{
	// �������

	// �s��̉��
	m_constBuff->Unmap(0, nullptr);
	// �萔�o�b�t�@�̉��
	SAFE_RELEASE(m_constBuff);
	// �e�X�g�e�N�X�`���̉��
	SAFE_RELEASE(m_pBasicDescHeap);
	// �e�N�X�`���o�b�t�@�̉��
	SAFE_RELEASE(m_texBuff);
	// ���[�g�V�O�l�`���̉��
	SAFE_RELEASE(m_rootSignature);
	// �p�C�v���C���X�e�[�g�̉��
	SAFE_RELEASE(m_pipelineState);
	// VS, PS�̉��
	SAFE_RELEASE(m_vsBlob);
	SAFE_RELEASE(m_psBlob);
	// ���_�o�b�t�@�̉��
	SAFE_RELEASE(m_vertexBuff);
	// �C���f�b�N�X�o�b�t�@�̉��
	SAFE_RELEASE(m_indexBuff);
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
	// ���_�o�b�t�@�̍쐬
	if (!CreateVertexBuffer()) { return false; }
	// �V�F�[�_�[�̍쐬
	if (!CreateShader()) { return false; }
	// �C���v�b�g���C�A�E�g�̍쐬
	if (!CreateInputLayout()) { return false; }
	// ���[�g�V�O�l�`���̍쐬
	if (!CreateRootSignature()) { return false; }
	// �p�C�v���C���X�e�[�g�̍쐬
	if (!CreatePipelineState()) { return false; }
	// �r���[�|�[�g�̍쐬
	if (!CreateViewport()) { return false; }
	// �e�X�g�e�N�X�`���̍쐬
	// if (!CreateTestTexture()) { return false; }
	// ���f���̓ǂݍ���
	if (!LoadModel()) { return false; }
	// �e�N�X�`���̍쐬
	if (!CreateTexture()) { return false; }
	// �萔�o�b�t�@�̍쐬
	if (!CreateConstantBuffer()) { return false; }
	// �V�F�[�_�[���\�[�X�r���[�̍쐬
	if (!CreateShaderResourceView()) { return false; }

	// ����ɏ������ł���
	return true;
}

void Renderer::Update()
{
	//// �萔�o�b�t�@�̐ݒ�
	//XMMATRIX world = XMMatrixIdentity();
	//XMMATRIX view = XMMatrixLookAtLH(
	//	XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	//);
	//XMMATRIX proj = XMMatrixPerspectiveFovLH(
	//	XM_PIDIV4,
	//	static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
	//	1.0f,
	//	10.0f
	//);

	//static float angle;
	//angle += 0.1f;
	//world = XMMatrixRotationY(angle);
	//// ���[���h�r���[�v���W�F�N�V�����s��
	//XMMATRIX wvp = world * view * proj;
	//*m_pMapMat = wvp;
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

	// �p�C�v���C���X�e�[�g�̐ݒ�
	m_cmdList->SetPipelineState(m_pipelineState);	                        // �p�C�v���C���X�e�[�g�̐ݒ�

	// �����_�[�^�[�Q�b�g���w��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * bbIdx;
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// ��ʃN���A
	float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// ���F
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);


	// ----- �`�施�߂����� -----
	m_cmdList->RSSetViewports(1, &m_viewport);	                            // �r���[�|�[�g�̐ݒ�
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);	                    // �V�U�����O��`�̐ݒ�
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);	                // ���[�g�V�O�l�`���̐ݒ�
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �v���~�e�B�u�g�|���W�̐ݒ�
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBuffView);	                // ���_�o�b�t�@�̐ݒ�
	m_cmdList->IASetIndexBuffer(&m_indexBuffView);	                        // �C���f�b�N�X�o�b�t�@�̐ݒ�
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);	                // ���[�g�V�O�l�`���̐ݒ�
	m_cmdList->SetDescriptorHeaps(1, &m_pBasicDescHeap);	                    // �f�B�X�N���v�^�q�[�v�̐ݒ�
	m_cmdList->SetGraphicsRootDescriptorTable(
		0,																		// ���[�g�p�����[�^�[�C���f�b�N�X 
		m_pBasicDescHeap->GetGPUDescriptorHandleForHeapStart());				// �q�[�v�A�h���X
	m_cmdList->DrawInstanced(vertNum, 1, 0, 0);								// �`�施��

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

	// // SRGB �����_�[�^�[�Q�b�g�r���[�ݒ�
	// D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	// rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// SRGB �t�H�[�}�b�g(�K���}�␳����)
	// rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2D �e�N�X�`��

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		hr = m_swapChain->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(&m_backBuffers[idx]));
		if (FAILED(hr))
		{
			MessageBox(nullptr, "GetBuffer Failed.", "Error", MB_OK);
			return false;
		}
		m_dev->CreateRenderTargetView(m_backBuffers[idx], nullptr, handle); // �K���}�␳�����������Ƃ��͑�������rtvDesc���w��
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

bool Renderer::CreateVertexBuffer()
{
	// ���_���̐ݒ�
	//Vertex vertices[] =
	//{
	//	{ { -1.0f, -1.0f, 0.0f}, { 0.0f, 1.0f } },	// ����
	//	{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },	// ����
	//	{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },	// �E��
	//	{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },	// �E��
	//};

	// �o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	                    // �A�b�v���[�h�q�[�v
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPU�y�[�W�v���p�e�B
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// �������v�[���̐ݒ�

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// ���\�[�X�̎���
	resDesc.Width = vertices.size();	                    // ���_�����邾���̃T�C�Y
	resDesc.Height = 1;	                                    // ����
	resDesc.DepthOrArraySize = 1;	                        // �f�v�X
	resDesc.MipLevels = 1;	                                // �~�b�v�}�b�v
	resDesc.Format = DXGI_FORMAT_UNKNOWN;	                // �t�H�[�}�b�g
	resDesc.SampleDesc.Count = 1;	                        // �T���v����
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	            // �t���O
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	    // �e�N�X�`���̃��C�A�E�g

	// ���\�[�X�̐���
	auto hr = m_dev->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuff));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// �o�b�t�@�r���[�̐ݒ�
	m_vertexBuffView.BufferLocation = m_vertexBuff->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	m_vertexBuffView.SizeInBytes = vertices.size();	                    // �S�o�C�g��
	m_vertexBuffView.StrideInBytes = pmdvertex_size;	                // 1���_������̃o�C�g��

	// �o�b�t�@�Ƀf�[�^���R�s�[
	unsigned char* vertMap = nullptr;
	hr = m_vertexBuff->Map(0, nullptr, (void**)&vertMap);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "VertexMap Failed.", "Error", MB_OK);
		return false;
	}
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	m_vertexBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�̐ݒ�
	unsigned short indices[6] = { 0, 1, 2, 2, 1, 3 };

	// �ݒ�́A�o�b�t�@�T�C�Y�ȊO�͒��_�o�b�t�@�Ɠ���
	resDesc.Width = sizeof(indices);	// �C���f�b�N�X�����邾���̃T�C�Y

	// ���\�[�X�̐���
	hr = m_dev->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_indexBuff));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// �o�b�t�@�Ƀf�[�^���R�s�[
	unsigned short* mappedIdx = nullptr;
	m_indexBuff->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	m_indexBuff->Unmap(0, nullptr);

	// �o�b�t�@�r���[�̐ݒ�
	m_indexBuffView.BufferLocation = m_indexBuff->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	m_indexBuffView.Format = DXGI_FORMAT_R16_UINT;	                        // �C���f�b�N�X�̃t�H�[�}�b�g
	m_indexBuffView.SizeInBytes = sizeof(indices);	                        // �o�b�t�@�̃T�C�Y

	return true;
}

bool Renderer::CreateShader()
{
	// �V�F�[�_�[�̃R���p�C��
	ID3DBlob* errorBlob = nullptr;

	// VS�̃R���p�C��
	auto hr = D3DCompileFromFile(
		L"Sources/HLSL/VertexShader/VS_Basic.hlsl",	        // �V�F�[�_�[�t�@�C��
		nullptr,			                                // �}�N���͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	                // �C���N���[�h�t�@�C���̎w��,����͕W��
		"VS_Main", "vs_5_0",	                            // �G���g���[�|�C���g�ƃo�[�W����
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�O�p�A�œK���̓X�L�b�v
		0,	                                                // �t���O
		&m_vsBlob,	                                        // �R���p�C����̃f�[�^
		&errorBlob	                                        // �G���[���b�Z�[�W
	);
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W�̕\��
		string errStr;	// ���p�̕�����
		errStr.resize(errorBlob->GetBufferSize());	// �G���[���b�Z�[�W�̃T�C�Y���擾

		// �f�[�^�R�s�[
		copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());
		errStr += "\n";

		// �G���[���b�Z�[�W�̕\��
		MessageBoxA(nullptr, errStr.c_str(), "Error", MB_OK);
		OutputDebugStringA(errStr.c_str());
		errorBlob->Release();

		return false;
	}

	// PS�̃R���p�C��
	hr = D3DCompileFromFile(
		L"Sources/HLSL/PixelShader/PS_Basic.hlsl",	        // �V�F�[�_�[�t�@�C��
		nullptr,			                                // �}�N���͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	                // �C���N���[�h�t�@�C���̎w��,����͕W��
		"PS_Main", "ps_5_0",	                            // �G���g���[�|�C���g�ƃo�[�W����
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�O�p�A�œK���̓X�L�b�v
		0,	                                                // �t���O
		&m_psBlob,	                                        // �R���p�C����̃f�[�^
		&errorBlob	                                        // �G���[���b�Z�[�W
	);
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W�̕\��
		string errStr;	// ���p�̕�����
		errStr.resize(errorBlob->GetBufferSize());	// �G���[���b�Z�[�W�̃T�C�Y���擾

		// �f�[�^�R�s�[
		copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());
		errStr += "\n";

		// �G���[���b�Z�[�W�̕\��
		MessageBoxA(nullptr, errStr.c_str(), "Error", MB_OK);
		OutputDebugStringA(errStr.c_str());
		errorBlob->Release();

		return false;
	}
	return true;
}

bool Renderer::CreateInputLayout()
{
	// �C���v�b�g���C�A�E�g�̐ݒ�
	m_inputLayout[0].SemanticName = "POSITION";	                                    // �Z�}���e�B�b�N��
	m_inputLayout[0].SemanticIndex = 0;	                                            // �Z�}���e�B�b�N�C���f�b�N�X
	m_inputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	                        // �t�H�[�}�b�g
	m_inputLayout[0].InputSlot = 0;	                                                // ���̓X���b�g
	m_inputLayout[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // �I�t�Z�b�g
	m_inputLayout[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���̓X���b�g�̃N���X
	m_inputLayout[0].InstanceDataStepRate = 0;	                                    // �C���X�^���X�f�[�^�̃X�e�b�v���[�g

	m_inputLayout[1].SemanticName = "NORMAL";	                                    // �Z�}���e�B�b�N��
	m_inputLayout[1].SemanticIndex = 0;	                                            // �Z�}���e�B�b�N�C���f�b�N�X
	m_inputLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;                          // �t�H�[�}�b�g
	m_inputLayout[1].InputSlot = 0;	                                                // ���̓X���b�g
	m_inputLayout[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // �I�t�Z�b�g
	m_inputLayout[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���̓X���b�g�̃N���X
	m_inputLayout[1].InstanceDataStepRate = 0;	                                    // �C���X�^���X�f�[�^�̃X�e�b�v���[�g

	m_inputLayout[2].SemanticName = "TEXCCORD";	                                    // �Z�}���e�B�b�N��
	m_inputLayout[2].SemanticIndex = 0;	                                            // �Z�}���e�B�b�N�C���f�b�N�X
	m_inputLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;		                        // �t�H�[�}�b�g
	m_inputLayout[2].InputSlot = 0;	                                                // ���̓X���b�g
	m_inputLayout[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // �I�t�Z�b�g
	m_inputLayout[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���̓X���b�g�̃N���X
	m_inputLayout[2].InstanceDataStepRate = 0;	                                    // �C���X�^���X�f�[�^�̃X�e�b�v���[�g

	m_inputLayout[3].SemanticName = "BONE_NO";	                                    // �Z�}���e�B�b�N��
	m_inputLayout[3].SemanticIndex = 0;	                                            // �Z�}���e�B�b�N�C���f�b�N�X
	m_inputLayout[3].Format = DXGI_FORMAT_R16G16_UINT;		                        // �t�H�[�}�b�g
	m_inputLayout[3].InputSlot = 0;	                                                // ���̓X���b�g
	m_inputLayout[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // �I�t�Z�b�g
	m_inputLayout[3].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���̓X���b�g�̃N���X
	m_inputLayout[3].InstanceDataStepRate = 0;	                                    // �C���X�^���X�f�[�^�̃X�e�b�v���[�g

	m_inputLayout[4].SemanticName = "WEIGHT";	                                    // �Z�}���e�B�b�N��
	m_inputLayout[4].SemanticIndex = 0;	                                            // �Z�}���e�B�b�N�C���f�b�N�X
	m_inputLayout[4].Format = DXGI_FORMAT_R8_UINT;			                        // �t�H�[�}�b�g
	m_inputLayout[4].InputSlot = 0;	                                                // ���̓X���b�g
	m_inputLayout[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // �I�t�Z�b�g
	m_inputLayout[4].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���̓X���b�g�̃N���X
	m_inputLayout[4].InstanceDataStepRate = 0;	                                    // �C���X�^���X�f�[�^�̃X�e�b�v���[�g

	m_inputLayout[5].SemanticName = "EDGE_FLG";	                                    // �Z�}���e�B�b�N��
	m_inputLayout[5].SemanticIndex = 0;	                                            // �Z�}���e�B�b�N�C���f�b�N�X
	m_inputLayout[5].Format = DXGI_FORMAT_R8_UINT;			                        // �t�H�[�}�b�g
	m_inputLayout[5].InputSlot = 0;	                                                // ���̓X���b�g
	m_inputLayout[5].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // �I�t�Z�b�g
	m_inputLayout[5].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// ���̓X���b�g�̃N���X
	m_inputLayout[5].InstanceDataStepRate = 0;	                                    // �C���X�^���X�f�[�^�̃X�e�b�v���[�g

	return true;
}

bool Renderer::CreateRootSignature()
{
	// �e�N�X�`���p���W�X�^�[0��
	D3D12_DESCRIPTOR_RANGE descRange[2] = {};
	descRange[0].NumDescriptors = 1;	// �e�N�X�`�����
	descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ��ʂ̓e�N�X�`��
	descRange[0].BaseShaderRegister = 0;	// 0�ԃX���b�g����
	descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// �I�t�Z�b�g�͒ǉ�

	// �萔�o�b�t�@�p���W�X�^�[0��
	descRange[1].NumDescriptors = 1;	// �萔�o�b�t�@���
	descRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// ��ʂ͒萔�o�b�t�@
	descRange[1].BaseShaderRegister = 0;	// 0�ԃX���b�g����
	descRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// �I�t�Z�b�g�͒ǉ�

	// ���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// �f�B�X�N���v�^�e�[�u��
	// �z��擪�A�h���X
	rootParam.DescriptorTable.pDescriptorRanges = &descRange[0];
	// �f�B�X�N���v�^�����W��
	rootParam.DescriptorTable.NumDescriptorRanges = 2;
	// ���ׂẴV�F�[�_�[���猩����
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					// �������̌J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					// �c�����̌J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					// ���s���̌J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// �{�[�_�[�͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					// ���`���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									// �~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;												// �~�b�v�}�b�v�ŏ��l
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			// �s�N�Z���V�F�[�_�[���猩����
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;				// ���T���v�����O���Ȃ�

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// ���̓A�Z���u�����̓��C�A�E�g������

	// ���[�g�p�����[�^�̐ݒ�
	rootSigDesc.pParameters = &rootParam;	// ���[�g�p�����[�^�̐擪�A�h���X
	rootSigDesc.NumParameters = 1;			// ���[�g�p�����[�^�̐�

	// �T���v���[�̐ݒ�
	rootSigDesc.pStaticSamplers = &samplerDesc;	// �T���v���[�̐ݒ�
	rootSigDesc.NumStaticSamplers = 1;			// �T���v���[�̐�

	// �o�C�i���R�[�h�̍쐬
	ID3DBlob* errorBlob = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	auto hr = D3D12SerializeRootSignature(
		&rootSigDesc,					// ���[�g�V�O�l�`���̐ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ���[�g�V�O�l�`���o�[�W����
		&signatureBlob,				// �o�C�i���f�[�^
		&errorBlob						// �G���[���b�Z�[�W
	);
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W�̕\��
		string errStr;	// ���p�̕�����
		errStr.resize(errorBlob->GetBufferSize());	// �G���[���b�Z�[�W�̃T�C�Y���擾

		// �f�[�^�R�s�[
		copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());
		errStr += "\n";

		// �G���[���b�Z�[�W�̕\��
		MessageBoxA(nullptr, errStr.c_str(), "Error", MB_OK);
		OutputDebugStringA(errStr.c_str());
		errorBlob->Release();

		return false;
	}

	// ���[�g�V�O�l�`���̐���
	hr = m_dev->CreateRootSignature(
		0,									// �m�[�h�}�X�N�B0�ŗǂ�	
		signatureBlob->GetBufferPointer(),	// �o�C�i���f�[�^
		signatureBlob->GetBufferSize(),		// �o�C�i���f�[�^�̃T�C�Y
		IID_PPV_ARGS(&m_rootSignature));	// ���[�g�V�O�l�`��

	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateRootSignature Failed.", "Error", MB_OK);
		signatureBlob->Release();
		return false;
	}
	signatureBlob->Release();	// �o�C�i���f�[�^�̉��
	return true;
}

bool Renderer::CreatePipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_rootSignature;	// ���[�g�V�O�l�`��
	// �V�F�[�_�[�̐ݒ�
	psoDesc.VS.pShaderBytecode = m_vsBlob->GetBufferPointer();	// VS�̃o�C�g�R�[�h
	psoDesc.VS.BytecodeLength = m_vsBlob->GetBufferSize();	    // VS�̃o�C�g�R�[�h�̃T�C�Y
	psoDesc.PS.pShaderBytecode = m_psBlob->GetBufferPointer();	// PS�̃o�C�g�R�[�h
	psoDesc.PS.BytecodeLength = m_psBlob->GetBufferSize();	    // PS�̃o�C�g�R�[�h�̃T�C�Y
	// �T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	            // �T���v���}�X�N
	psoDesc.RasterizerState.MultisampleEnable = false;	        // �A���`�G�C���A�X�͎g��Ȃ��̂�false
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// �J�����O���[�h
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// ���g��h��Ԃ�
	psoDesc.RasterizerState.DepthClipEnable = true;	            // �[�x�N���b�s���O��L���ɂ���
	// �u�����h�X�e�[�g�̐ݒ�
	psoDesc.BlendState.AlphaToCoverageEnable = false;	        // �A���t�@�c�[�J�o���b�W�͎g��Ȃ�
	psoDesc.BlendState.IndependentBlendEnable = false;	        // �Ɨ��u�����h�͎g��Ȃ�
	// �����_�[�^�[�Q�b�g�̐ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = false;				                    // �u�����h���Ȃ�
	rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// ���ׂẴJ���[�`�����l������������
	psoDesc.BlendState.RenderTarget[0] = rtBlendDesc;	                // �����_�[�^�[�Q�b�g�̐ݒ�
	psoDesc.NumRenderTargets = 1;	                                    // �����_�[�^�[�Q�b�g�̐�
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;					// �����_�[�^�[�Q�b�g�̃t�H�[�}�b�g
	// �C���v�b�g���C�A�E�g�̐ݒ�
	psoDesc.InputLayout.pInputElementDescs = m_inputLayout;		// �C���v�b�g���C�A�E�g
	psoDesc.InputLayout.NumElements = _countof(m_inputLayout);	// �C���v�b�g���C�A�E�g�̐�
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// �J�b�g�Ȃ�
	// �v���~�e�B�u�g�|���W�̐ݒ�
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// �O�p�`���X�g
	// �A���`�G�C���A�V���O�̂��߂̃T���v���W���̐ݒ�
	psoDesc.SampleDesc.Count = 1;	// �T���v�����O��1�s�N�Z���ɂ�1��
	psoDesc.SampleDesc.Quality = 0;	// �N�I���e�B�͍Œ�

	// �p�C�v���C���X�e�[�g�̐���
	auto hr = m_dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateGraphicsPipelineState Failed.", "Error", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::CreateViewport()
{
	// �r���[�|�[�g�̐ݒ�
	m_viewport.Width = static_cast<float>(WINDOW_WIDTH);	    // �E�B���h�E�̕�
	m_viewport.Height = static_cast<float>(WINDOW_HEIGHT);	    // �E�B���h�E�̍���
	m_viewport.TopLeftX = 0;	                                // �o�͐�̍����X���W
	m_viewport.TopLeftY = 0;	                                // �o�͐�̍����Y���W
	m_viewport.MaxDepth = 1.0f;	                                // �[�x�l�̍ő�l
	m_viewport.MinDepth = 0.0f;	                                // �[�x�l�̍ŏ��l

	// �V�U�����O��`�̐ݒ�
	m_scissorRect.top = 0;	                                    // �؂蔲����`�̏�[
	m_scissorRect.left = 0;	                                    // �؂蔲����`�̍��[
	m_scissorRect.right = m_scissorRect.left + WINDOW_WIDTH;	// �؂蔲����`�̉E�[
	m_scissorRect.bottom = m_scissorRect.top + WINDOW_HEIGHT;	// �؂蔲����`�̉��[

	return true;
}

bool Renderer::CreateTestTexture()
{
	// �e�N�X�`���̐ݒ�
	m_texData.resize(256 * 256);	// �e�N�X�`���f�[�^�̃T�C�Y

	for (auto& tex : m_texData)
	{
		tex.r = rand() % 256;	// �Ԑ���
		tex.g = rand() % 256;	// �ΐ���
		tex.b = rand() % 256;	// ����
		tex.a = 255;	        // �A���t�@������1.0�Ƃ���
	}

	// WriteToSubresource�œ]�����邽�߂̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// ����Ȑݒ�Ȃ̂�DEFAULT�ł�UPLOAD�ł��Ȃ�
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ���C�g�o�b�N
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// �]����L0�A�܂�CPu�����璼�ڍs��
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// �P��A�_�v�^�̂���0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// �t�H�[�}�b�g
	resDesc.Width = 256;									// ��
	resDesc.Height = 256;									// ����
	resDesc.DepthOrArraySize = 1;							// 2D�Ŕz����Ȃ��̂�1
	resDesc.SampleDesc.Count = 1;							// �ʏ�e�N�X�`���Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�
	resDesc.SampleDesc.Quality = 0;							// �N�I���e�B�͍Œ�
	resDesc.MipLevels = 1;									// �~�b�v�}�b�v�͎g��Ȃ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	        // ���C�A�E�g�͌��肵�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;				// ���Ƀt���O�͂Ȃ�

	// ���\�[�X�̐���
	m_texBuff = nullptr;

	auto hr = m_dev->CreateCommittedResource(
		&heapProp,									// �q�[�v�̐ݒ�
		D3D12_HEAP_FLAG_NONE,						// ���Ƀt���O�͂Ȃ�
		&resDesc,									// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �e�N�X�`���p�w��
		nullptr,									// �N���A�l
		IID_PPV_ARGS(&m_texBuff)						// �e�N�X�`���o�b�t�@
	);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// �f�[�^�]��
	hr = m_texBuff->WriteToSubresource(
		0,
		nullptr,				// �S�̈�ւ̃R�s�[
		m_texData.data(),		// ���f�[�^�A�h���X
		sizeof(TexRGBA) * 256,	// 1�s���̃T�C�Y
		sizeof(TexRGBA) * m_texData.size()	// �S�f�[�^�T�C�Y
	);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "WriteToSubresource Failed.", "Error", MB_OK);
		return false;
	}

	return true;

	return true;
}

bool Renderer::CreateShaderResourceView()
{
	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	m_pBasicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

	// �V�F�[�_�[���猩����悤��
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �}�X�N��0
	descHeapDesc.NodeMask = 0;
	// SRV��CBV��2��
	descHeapDesc.NumDescriptors = 2;
	// �V�F�[�_�[���\�[�X�r���[�p
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	// ����
	auto hr = m_dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_pBasicDescHeap));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateDescriptorHeap Failed.", "Error", MB_OK);
		return false;
	}

	// �V�F�[�_�[���\�[�X�r���[�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(0.0f�`1.0f�ɐ��K��)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// �}�b�s���O
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;						// �~�b�v�}�b�v�͎g��Ȃ�

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = m_pBasicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// �V�F�[�_�[���\�[�X�r���[�̐���
	m_dev->CreateShaderResourceView(m_texBuff, &srvDesc, basicHeapHandle);

	// ���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �萔�o�b�t�@�r���[�̐ݒ�
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_constBuff->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	cbvDesc.SizeInBytes = m_constBuff->GetDesc().Width;	    // �o�b�t�@�̃T�C�Y

	// �萔�o�b�t�@�r���[�̐���
	m_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return true;
}

bool Renderer::CreateTexture()
{
	TexMetadata meta = {};
	ScratchImage img = {};

	auto hr = LoadFromWICFile(L"Assets/Texture/field.jpg", WIC_FLAGS_NONE, &meta, img);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "LoadFromWICFile Failed.", "Error", MB_OK);
		return false;
	}

	auto imgData = img.GetImage(0, 0, 0);	// ���f�[�^���o

	// WriteToSubresource�œ]�����邽�߂̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// ����Ȑݒ�Ȃ̂�DEFAULT�ł�UPLOAD�ł��Ȃ�
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ���C�g�o�b�N
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// �]����L0�A�܂�CPu�����璼�ڍs��
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// �P��A�_�v�^�̂���0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// �t�H�[�}�b�g
	resDesc.Width = meta.width;								// ��
	resDesc.Height = meta.height;							// ����
	resDesc.DepthOrArraySize = meta.arraySize;				// 2D�Ŕz����Ȃ��̂�1
	resDesc.SampleDesc.Count = 1;							// �ʏ�e�N�X�`���Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�
	resDesc.SampleDesc.Quality = 0;							// �N�I���e�B�͍Œ�
	resDesc.MipLevels = meta.mipLevels;						// �~�b�v���x��
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(meta.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	        // ���C�A�E�g�͌��肵�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;				// ���Ƀt���O�͂Ȃ�

	// ���\�[�X�̐���
	m_texBuff = nullptr;

	hr = m_dev->CreateCommittedResource(
		&heapProp,									// �q�[�v�̐ݒ�
		D3D12_HEAP_FLAG_NONE,						// ���Ƀt���O�͂Ȃ�
		&resDesc,									// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �e�N�X�`���p�w��
		nullptr,									// �N���A�l
		IID_PPV_ARGS(&m_texBuff)						// �e�N�X�`���o�b�t�@
	);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// �f�[�^�]��
	hr = m_texBuff->WriteToSubresource(
		0,
		nullptr,				// �S�̈�ւ̃R�s�[
		imgData->pixels,		// ���f�[�^�A�h���X
		imgData->rowPitch,		// 1�s���̃T�C�Y
		imgData->slicePitch		// �S�f�[�^�T�C�Y
	);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "WriteToSubresource Failed.", "Error", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::CreateConstantBuffer()
{
	// ���[���h�s��̐ݒ�
	XMMATRIX matrix = XMMatrixIdentity();
	XMMATRIX world = XMMatrixRotationY(XMConvertToRadians(45.0f));
	XMFLOAT3 eye(0, 10, -15);
	XMFLOAT3 target(0, 10, 0);
	XMFLOAT3 up(0, 1, 0);
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 1.0f, 100.0f);

	// �o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	                    // �A�b�v���[�h�q�[�v
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPU�y�[�W�v���p�e�B
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// �������v�[���̐ݒ�

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// ���\�[�X�̎���
	resDesc.Width = (sizeof(matrix) + 0xff) & ~0xff;	    // ���_�����邾���̃T�C�Y
	resDesc.Height = 1;	                                    // ����
	resDesc.DepthOrArraySize = 1;	                        // �f�v�X
	resDesc.MipLevels = 1;	                                // �~�b�v�}�b�v
	resDesc.Format = DXGI_FORMAT_UNKNOWN;	                // �t�H�[�}�b�g
	resDesc.SampleDesc.Count = 1;	                        // �T���v����
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	            // �t���O
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	    // �e�N�X�`���̃��C�A�E�g


	auto hr = m_dev->CreateCommittedResource(
		&heapProp,		// �A�b�v���[�h�q�[�v
		D3D12_HEAP_FLAG_NONE,								// ���Ƀt���O�͂Ȃ�
		&resDesc,			// �o�b�t�@�̐ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,					// ���\�[�X�̏��
		nullptr,											// �N���A�l
		IID_PPV_ARGS(&m_constBuff)							// �o�b�t�@
	);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// �o�b�t�@�Ƀf�[�^���R�s�[
	m_pMapMat = nullptr;	// �}�b�v��������|�C���^�[
	hr = m_constBuff->Map(0, nullptr, (void**)&m_pMapMat);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Map Failed.", "Error", MB_OK);
		return false;
	}

	// �s��̍���
	matrix = world * view * proj;
	*m_pMapMat = matrix;	// �s��̓��e���R�s�[

	return true;
}

bool Renderer::LoadModel()
{

	// ���f���̓ǂݍ���
	char signature[3] = {};	// �V�O�l�`��
	FILE* fp = fopen("Assets/Model/�����~�N.pmd", "rb");

	fread(signature, sizeof(signature), 1, fp);
	fread(&m_pmdHeader, sizeof(m_pmdHeader), 1, fp);


	fread(&vertNum, sizeof(vertNum), 1, fp);

	vertices.resize(vertNum * pmdvertex_size);	// �o�b�t�@�[�̊m��
	fread(vertices.data(), vertices.size(), 1, fp);	// �ǂݍ���


	fclose(fp);

	return true;
}

