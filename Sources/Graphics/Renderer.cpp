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
	if (!CreateRTV())
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

	// �R�}���h���X�g�����
	m_pCmdList->Close();

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
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();	// 
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	// �J�ڑO�̃��\�[�X�̏�� : �v���[���g
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �J�ڌ�̃��\�[�X�̏�� : �����_�[�^�[�Q�b�g
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// �T�u���\�[�X : �S��

	// ���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g�̐ݒ�
	m_pCmdList->OMSetRenderTargets(1, &m_RTVHandle[m_FrameIndex], FALSE, nullptr);

	// �N���A�J���[
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	// �����_�[�^�[�Q�b�g�r���[���N���A
	m_pCmdList->ClearRenderTargetView(m_RTVHandle[m_FrameIndex], clearColor, 0, nullptr);

}

void Renderer::End()
{
	// ���\�[�X�o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// ���\�[�X�o���A�̎�� : ���\�[�X�̏�ԑJ��
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// �t���O :
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();	// ���\�[�X
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
	// �X���b�v�`�F�C���̕\��
	// ��1���� : �t���[���̕\���Ԋu : 1�ōX�V�����ʂ�ɕ\���A0�ő����\��(2�Ȃǂł�2��ڂ̐���������ɕ\���𓯊� ��:�f�B�X�v���C�̍X�V������60Hz�̏ꍇ�A2��30FPS�ƂȂ�)
	// ��2���� : �t���O : ���ɂȂ�
	m_pSwapChain->Present(interval, 0);

	// �V�O�i������
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pCmdQueue->Signal(m_pFence.Get(), currentValue);

	// �o�b�N�o�b�t�@�ԍ����X�V
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// ���̃t���[���̕`�揀�����c�ł���Αҋ@����
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_fenceEvent);
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// ���̃t���[���̃t�F���X�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex]++;
}

void Renderer::Uninit()
{
	// GPU�̏���������҂�
	WaitGpu();

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

bool Renderer::CreateRTV()
{
	// ====== �����_�[�^�[�Q�b�g�r���[(RTV) ======
	// �����_�[�^�[�Q�b�g��Direct3D�̕`���
	// ���̂̓o�b�N�o�b�t�@��e�N�X�`���Ƃ��������\�[�X
	// ���\�[�X�͒P�Ƀ������̈�����I�u�W�F�N�g���킩���Ă��邾����GPU�̃A�h���X���ǂ��ɂȂ�̂��킩��Ȃ�
	// �܂��A�o�b�t�@�v�̂͂킩���Ă��邪�A�o�b�t�@�̋�؂���ɂ����2�����̃e�N�X�`���Ƃ��Ďg���̂��A3�����̃e�N�X�`���Ƃ��Ďg���̂��킩��Ȃ�
	// �����ŁA���\�[�X���ǂ̂悤�Ɏg���̂����w�肷��̂��r���[
	// RTV�̍쐬���@
	// 1,���\�[�X�̐���
	// 2,�f�B�X�N���v�^�q�[�v�̐���
	// 3,�����_�[�^�[�Q�b�g�r���[�̐���
	// ����̏ꍇ�̓X���b�v�`�F�C���̃o�b�N�o�b�t�@�����\�[�X�Ƃ��邽�߁A1�͕s�v
	// ���ɂ��ׂ����Ƃ̓f�B�X�N���v�^�̃A�h���X�擾

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FRAME_BUFFER_COUNT;	// �f�B�X�N���v�^�� : �o�b�t�@��
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// �q�[�v�̎��	: RTV
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �t���O : ���ɂȂ�
	desc.NodeMask = 0;	// �m�[�h�}�X�N : GPU����������ꍇ�Ɏg�p

	// �f�B�X�N���v�^�q�[�v�̐���
	auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pRTVHeap.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr)) { return false; }

	// �����_�[�^�[�Q�b�g�r���[�̐���
	auto handle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();
	// ���̊֐��Ŏ擾�ł���l�͌Œ�l�����A�f�o�C�X�ˑ��̂��ߕK���擾����
	auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].ReleaseAndGetAddressOf()));
		// �G���[�`�F�b�N
		if (FAILED(hr)) { return false; }

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// �t�H�[�}�b�g : sRGB�ł�RGBA8bit
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// �r���[�̎��� : 2D
		viewDesc.Texture2D.MipSlice = 0;	// �~�b�v�}�b�v�X���C�X : 0
		viewDesc.Texture2D.PlaneSlice = 0;	// �v���[���X���C�X : 0

		m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);
		m_RTVHandle[i] = handle;
		handle.ptr += incrementSize;
	}

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
