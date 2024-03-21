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
{
}

Renderer::~Renderer()
{
	// �������

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

	// �p�C�v���C���X�e�[�g�̐ݒ�
	m_cmdList->SetPipelineState(m_pipelineState);	                        // �p�C�v���C���X�e�[�g�̐ݒ�

	// �����_�[�^�[�Q�b�g���w��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * bbIdx;
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// ��ʃN���A
	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };	// ���F
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);


	// ----- �`�施�߂����� -----
	m_cmdList->RSSetViewports(1, &m_viewport);	                            // �r���[�|�[�g�̐ݒ�
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);	                    // �V�U�����O��`�̐ݒ�
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);	                // ���[�g�V�O�l�`���̐ݒ�
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �v���~�e�B�u�g�|���W�̐ݒ�
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBuffView);	                // ���_�o�b�t�@�̐ݒ�
	m_cmdList->IASetIndexBuffer(&m_indexBuffView);	                        // �C���f�b�N�X�o�b�t�@�̐ݒ�
	m_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);	                        // �`�施��

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

bool Renderer::CreateVertexBuffer()
{
	// ���_���̐ݒ�
	XMFLOAT3 vertices[4] =
	{
		{-0.4f, -0.7f, 0.0f},	// ����
		{-0.4f, 0.7f, 0.0f},	// ����
		{0.4f, -0.7f, 0.0f},	// �E��
		{0.4f, 0.7f, 0.0f},		// �E��
	};

	// �o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	                    // �A�b�v���[�h�q�[�v
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPU�y�[�W�v���p�e�B
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// �������v�[���̐ݒ�

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// ���\�[�X�̎���
	resDesc.Width = sizeof(vertices);	                    // ���_�����邾���̃T�C�Y
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

	// �o�b�t�@�Ƀf�[�^���R�s�[
	void* p = nullptr;
	m_vertexBuff->Map(0, nullptr, &p);
	memcpy(p, vertices, sizeof(vertices));
	m_vertexBuff->Unmap(0, nullptr);

	// �o�b�t�@�r���[�̐ݒ�
	m_vertexBuffView.BufferLocation = m_vertexBuff->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	m_vertexBuffView.SizeInBytes = sizeof(vertices);	                    // �o�b�t�@�̃T�C�Y
	m_vertexBuffView.StrideInBytes = sizeof(XMFLOAT3);	                    // �X�g���C�h

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
	m_indexBuff->Map(0, nullptr, &p);
	memcpy(p, indices, sizeof(indices));
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

	return true;
}

bool Renderer::CreateRootSignature()
{
	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// ���̓A�Z���u�����̓��C�A�E�g������

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
