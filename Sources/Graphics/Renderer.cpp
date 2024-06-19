// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̎���
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Renderer.h"

void EnableDebugLayer()
{
	ComPtr<ID3D12Debug> debug;
	auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf()));

	// �f�o�b�O���C���[�̗L����
	if (SUCCEEDED(hr))
	{
		debug->EnableDebugLayer();
	}
}


bool Renderer::Init(Window* window)
{
	// �E�B���h�E�N���X�̃|�C���^��ݒ�
	m_pWindow = window;


#if defined(_DEBUG) || defined(DEBUG)
	// �f�o�b�O���C���[�̗L����
	EnableDebugLayer();
#endif	// _DEBUG || DEBUG

	// --- D3D������ ---
	// 1.�f�o�C�X�̐���
	if (!CreateDevice())
	{
		// �f�o�C�X�̐����Ɏ��s�����ꍇ
		assert(0 && "D3D�f�o�C�X�̐����Ɏ��s���܂���");
		return false;
	}

	// 2.�R�}���h�L���[�̐���
	if (!CreateCommandQueue())
	{
		// �R�}���h�L���[�̐����Ɏ��s�����ꍇ
		assert(0 && "�R�}���h�L���[�̐����Ɏ��s���܂���");
		return false;
	}

	// 3.�X���b�v�`�F�C���̐���
	if (!CreateSwapChain())
	{
		// �X���b�v�`�F�C���̐����Ɏ��s�����ꍇ
		assert(0 && "�X���b�v�`�F�C���̐����Ɏ��s���܂����B");
		return false;
	}

	if (!CreateDescriptorPool())
	{
		// �f�B�X�N���v�^�v�[���̐����Ɏ��s�����ꍇ
		assert(0 && "�f�B�X�N���v�^�v�[���̐����Ɏ��s���܂����B");
		return false;
	}

	// 4.�R�}���h�A���P�[�^�̐���
	if (!CreateCommandAllocator())
	{
		// �R�}���h�A���P�[�^�̐����Ɏ��s�����ꍇ
		assert(0 && "�R�}���h�A���P�[�^�̐����Ɏ��s���܂����B");
		return false;

	}

	// 5.�R�}���h���X�g�̐���
	if (!CreateCommandList())
	{
		// �R�}���h���X�g�̐����Ɏ��s�����ꍇ
		assert(0 && "�R�}���h���X�g�̐����Ɏ��s���܂����B");
		return false;

	}

	// 6.RTV�̐���
	if (!CreateRenderTargetView())
	{
		// RTV�̐����Ɏ��s�����ꍇ
		assert(0 && "RTV�̐����Ɏ��s���܂����B");
		return false;
	}

	// 7.�t�F���X�̐���
	if (!CreateFence())
	{
		// �t�F���X�̐����Ɏ��s�����ꍇ
		assert(0 && "�t�F���X�̐����Ɏ��s���܂����B");
		return false;
	}

	// 8.�[�x�X�e���V���o�b�t�@�̐���
	if (!CreateDepthStencilView())
	{
		// �[�x�X�e���V���o�b�t�@�̐����Ɏ��s�����ꍇ
		assert(0 && "�[�x�X�e���V���o�b�t�@�̐����Ɏ��s���܂����B");
		return false;
	}

	// �R�}���h���X�g�����
	m_pCmdList->Close();

	// �r���[�|�[�g�̐ݒ�.
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = float(WIDTH);
		m_viewport.Height = float(HEIGHT);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}

	// �V�U�[��`�̐ݒ�.
	{
		m_scissor.left = 0;
		m_scissor.right = WIDTH;
		m_scissor.top = 0;
		m_scissor.bottom = HEIGHT;
	}

	// ����������
	return true;
}

void Renderer::Begin()
{
	// �R�}���h�̋L�^���J�n
	m_pCmdAllocator[m_FrameIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex].Get(), nullptr);

	// ���\�[�X�o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// ���\�[�X�o���A�̎�� : ���\�[�X�̏�ԑJ��
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// �t���O : ���ɂȂ�
	barrier.Transition.pResource = m_ColorTarget[m_FrameIndex].GetResource();	// 
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	// �J�ڑO�̃��\�[�X�̏�� : �v���[���g
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �J�ڌ�̃��\�[�X�̏�� : �����_�[�^�[�Q�b�g
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// �T�u���\�[�X : �S��

	// ���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	auto handleRTV = m_ColorTarget[m_FrameIndex].GetHandleRTV();
	auto handleDSV = m_DepthTarget.GetHandleDSV();

	// �����_�[�^�[�Q�b�g�̐ݒ�
	m_pCmdList->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

	// �N���A�J���[
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	// �����_�[�^�[�Q�b�g�r���[���N���A
	m_pCmdList->ClearRenderTargetView(handleRTV->HandleCPU, clearColor, 0, nullptr);
	// �[�x�X�e���V���r���[���N���A.
	m_pCmdList->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Renderer::End()
{
	// ���\�[�X�o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// ���\�[�X�o���A�̎�� : ���\�[�X�̏�ԑJ��
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// �t���O :
	barrier.Transition.pResource = m_ColorTarget[m_FrameIndex].GetResource();	// ���\�[�X
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �J�ڑO�̃��\�[�X�̏��
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;	// �J�ڌ�̃��\�[�X�̏��
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// �T�u���\�[�X

	// ���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	// �R�}���h�̋L�^���I��
	m_pCmdList->Close();

	// �R�}���h�̎��s
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(1, ppCmdLists);

	// ��ʂɕ\��
	Present(1);
}

void Renderer::Present(uint32_t interval)
{
	// ��ʃt���b�v
	// ��1���� : �t���[���̕\���Ԋu : 1�ōX�V�����ʂ�ɕ\���A0�ő����\��(2�Ȃǂł�2��ڂ̐���������ɕ\���𓯊� ��:�f�B�X�v���C�̍X�V������60Hz�̏ꍇ�A2��30FPS�ƂȂ�)
	// ��2���� : �t���O : ���ɂȂ�
	m_pSwapChain->Present(interval, 0);

	// �V�O�i������.
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pCmdQueue->Signal(m_pFence.Get(), currentValue);

	// �o�b�N�o�b�t�@�ԍ����X�V.
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// ���̃t���[���̕`�揀�����܂��ł���Αҋ@����
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_fenceEvent);
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// ���̃t���[���̃t�F���X�J�E���^�[�𑝂₷.
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

void Renderer::Uninit()
{
	// GPU�̏���������҂�
	WaitGpu();

	// �����_�[�^�[�Q�b�g�r���[�̔j��.
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_ColorTarget[i].Uninit();
	}

	// �[�x�X�e���V���r���[�̔j��.
	m_DepthTarget.Uninit();

	// �f�B�X�N���v�^�v�[���̔j��.
	for (auto i = 0; i < POOL_COUNT; ++i)
	{
		if (m_pPool[i] != nullptr)
		{
			m_pPool[i]->Release();
			m_pPool[i] = nullptr;
		}
	}

	// �C�x���g�j��
	if (m_fenceEvent != nullptr)
	{
		CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;
	}

	m_pWindow = nullptr;
}

void Renderer::WaitGpu()
{
	assert(m_pCmdQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_fenceEvent != nullptr);

	// �V�O�i������
	m_pCmdQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	// �������ɃC�x���g��ݒ肷��
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_fenceEvent);

	// �C�x���g���ݒ肳���܂őҋ@
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// �t�F���X�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex]++;
}

bool Renderer::CreateDevice()
{
	// �f�o�C�X�̐���
	// ��1���� : �r�f�I�A�_�v�^�[�ւ̃|�C���^�B�f�t�H���g�̃A�_�v�^�[���g�p����ꍇ��nullptr
	// ��2���� : �f�o�C�X�̐����̂��߂ɕK�v�Ƃ����ŏ��@�\���x���BD3D_FEATURE_LEVEL�񋓌^�̒l���w��
	// ��3���� : �f�o�C�X���쐬���邽�߂̃C���^�[�t�F�[�XID
	// �}�N�����g�p���Ȃ��ꍇ
	// auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), reinterpret_cast<void**>(&m_pDevice));
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return false; }
	return true;
}

bool Renderer::CreateCommandQueue()
{
	// �R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// �R�}���h���X�g�̎�� : GPU�����s�\�ȃR�}���h�o�b�t�@���w��
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �R�}���h�L���[�̗D��x : �ʏ�
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// �R�}���h�L���[�̃t���O : ���ɂȂ�
	desc.NodeMask = 0;	// �m�[�h�}�X�N : GUP����������ꍇ�Ɏg�p

	// �R�}���h�L���[�̐���
	auto hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return false; }
	return true;
}

bool Renderer::CreateSwapChain()
{
	// DXGI�t�@�N�g���[�̐���
	// DXGI : DirectX�O���t�B�b�N�X�C���X�g���N�`���[
	// DXGI�̓J�[�l�����[�h�h���C�o�[�y�уV�X�e���n�[�h�E�F�A�Ƃ̒ʐM���s�����߂�API
	// DXGI���̂�Direct3D�Ɉˑ����Ȃ�
	// ��{�I�ɂ͂ǂ̂悤��GPU���ڑ�����Ă��邩�̗񋓂�A�`�悵���t���[������ʂ֕\�����邽�߂Ɏg��
	IDXGIFactory4* pFactory = nullptr;	// DXGI�t�@�N�g���[�C���^�[�t�F�[�X
	auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return false; }

	// �X���b�v�`�F�C���̐ݒ�
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferDesc.Width = m_pWindow->GetWidth();			// �𑜓x�̉������w��
	desc.BufferDesc.Height = m_pWindow->GetHeight();		// �𑜓x�̏c�����w��
	desc.BufferDesc.RefreshRate.Numerator = 60;				// ���t���b�V�����[�g�̕��q
	desc.BufferDesc.RefreshRate.Denominator = 1;			// ���t���b�V�����[�g�̕���
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// �X�L�������C���̏��� : ���Ɏw�肵�Ȃ�
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					// �X�P�[�����O : ���Ɏw�肵�Ȃ�
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// �t�H�[�}�b�g	: RGBA8bit
	desc.SampleDesc.Count = 1;								// �T���v����
	desc.SampleDesc.Quality = 0;							// �T���v���i��
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// �o�b�t�@�̎g�p�@ : �o�̓����_�[�^�[�Q�b�g�Ƃ��Ďg�p
	desc.BufferCount = FRAME_BUFFER_COUNT;					// �o�b�t�@��
	desc.OutputWindow = m_pWindow->GetHwnd();				// �o�͐�̃E�B���h�E�n���h��
	desc.Windowed = TRUE;									// �E�B���h�E���[�h
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// �X���b�v�G�t�F�N�g : �o�b�N�o�b�t�@�̓��e�𑬂₩�ɔj��
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �t���O : ���[�h�؂�ւ�������

	// �X���b�v�`�F�C���̐���
	IDXGISwapChain* pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(m_pCmdQueue.Get(), &desc, &pSwapChain);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		SAFE_RELEASE(pFactory);
		return false;
	}

	// IDXGISwapChain3���擾
	hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		SAFE_RELEASE(pFactory);
		SAFE_RELEASE(pSwapChain);
		return false;
	}

	// �o�b�N�o�b�t�@�ԍ����擾
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// �s�v�ɂȂ����̂ŉ��
	SAFE_RELEASE(pFactory);
	SAFE_RELEASE(pSwapChain);

	return true;
}

bool Renderer::CreateCommandAllocator()
{
	// �R�}���h�A���P�[�^�̐���
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		// ��1���� : �R�}���h���X�g�̎�� : GPU�����s�\�ȃR�}���h�o�b�t�@���w��
		auto hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCmdAllocator[i].ReleaseAndGetAddressOf()));
		// �G���[�`�F�b�N
		if (FAILED(hr)) { return false; }
	}
	return true;
}

bool Renderer::CreateCommandList()
{
	// �R�}���h���X�g�̐���
	// ��1���� : ������GPU�m�[�h������ꍇ�ɁA�m�[�h�����ʂ��邽�߂̃r�b�g�}�X�N : GPU��1�����Ȃ��ꍇ��0
	// ��2���� : �R�}���h���X�g�̎�� : GPU�����s�\�ȃR�}���h�o�b�t�@���w��
	// ��3���� : �R�}���h�A���P�[�^ : �R�}���h���X�g���g�p����R�}���h�A���P�[�^
	// ��4���� : �p�C�v���C���X�e�[�g : ��Őݒ肷�邽�߂�nullptr���w��
	// ��5���� : �C���^�[�t�F�[�XID
	auto hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAllocator[m_FrameIndex].Get(), nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return false; }
	return true;
}

bool Renderer::CreateFence()
{
	// CPU��GPU�̓���������Application���̐Ӗ�
	// �t�F���X��CPU��GPU�̓�������邽�߂̎d�g��
	// �t�F���X�̒l���C���N�������g���ꂽ���ǂ�����GPU�̏������I��������ǂ����𔻒f����

	// �t�F���X�J�E���^�[�����Z�b�g
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_FenceCounter[i] = 0;
	}

	// �t�F���X�̐���
	// ��1���� : �t�F���X�̏����l
	// ��2���� : �t�F���X�̃t���O : ���ɂȂ�
	// ��3���� : �C���^�[�t�F�[�XID
	auto hr = m_pDevice->CreateFence(m_FenceCounter[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return false; }

	m_FenceCounter[m_FrameIndex]++;

	// �t�F���X�C�x���g�̐���
	// �`�揈�������ۂɏI���܂ő҂���OS�̃C�x���g���g�p
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	// �G���[�`�F�b�N
	if (m_fenceEvent == nullptr) { return false; }

	return true;
}

bool Renderer::CreateRenderTargetView()
{
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (!m_ColorTarget[i].InitFromBackBuffer(m_pPool[POOL_TYPE_RTV], i))
		{
			ELOG("[Renderer.cpp]Error : Line 425 : �����_�[�^�[�Q�b�g�r���[�̐����Ɏ��s���܂����B");
			return false;
		}
	}
	return true;
}

bool Renderer::CreateDepthStencilView()
{
	if (!m_DepthTarget.Init(m_pPool[POOL_TYPE_DSV], WIDTH, HEIGHT, DXGI_FORMAT_D32_FLOAT))
	{
		ELOG("[Renderer.cpp]Error : Line 436 : �[�x�X�e���V���r���[�̐����Ɏ��s���܂����B");
		return false;
	}
	return true;
}

bool Renderer::CreateDescriptorPool()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	desc.NodeMask = 1;	// �m�[�h�}�X�N : 1
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// �q�[�v�̎�� : CBV/SRV/UAV
	desc.NumDescriptors = 512;	// �f�B�X�N���v�^�� : 512
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �t���O : �V�F�[�_�[���猩����
	if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_RES]))
	{
		ELOG("[Renderer.cpp]Error : Line 453 : �f�B�X�N���v�^�v�[��(RES)�̐����Ɏ��s���܂����B");
		return false;
	}

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;	// �q�[�v�̎�� : Sampler
	desc.NumDescriptors = 256;	// �f�B�X�N���v�^�� : 256
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �t���O : �V�F�[�_�[���猩����
	if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_SMP]))
	{
		ELOG("[Renderer.cpp]Error : Line 463 : �f�B�X�N���v�^�v�[��(SMP)�̐����Ɏ��s���܂����B");
		return false;
	}

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// �q�[�v�̎�� : RTV
	desc.NumDescriptors = 512;	// �f�B�X�N���v�^�� : 512
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �t���O : ���ɂȂ�
	if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_RTV]))
	{
		ELOG("[Renderer.cpp]Error : Line 473 : �f�B�X�N���v�^�v�[��(RTV)�̐����Ɏ��s���܂����B");
		return false;
	}

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	// �q�[�v�̎�� : DSV
	desc.NumDescriptors = 512;	// �f�B�X�N���v�^�� : 512
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �t���O : ���ɂȂ�
	if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_DSV]))
	{
		ELOG("[Renderer.cpp]Error : Line 483 : �f�B�X�N���v�^�v�[��(DSV)�̐����Ɏ��s���܂����B");
		return false;
	}

	// ����I��
	return true;
}
