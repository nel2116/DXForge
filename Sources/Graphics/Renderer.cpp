// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̎���
// �X�V����
// 2024/05/25 �쐬
// 2024/06/24 ���t�@�N�^�����O
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Renderer.h"
#include <System/d3dx12.h>

namespace
{
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

	// �����̈�̌v�Z
	inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return std::max(0, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0, std::min(ay2, by2) - std::max(ay1, by1));
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

	// �f�o�C�X�̐���.
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));
	if (FAILED(hr))
	{
		return false;
	}

	// �R�}���h�L���[�̐���.
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr))
		{
			return false;
		}
	}

	// �X���b�v�`�F�C���̐���.
	{
		// DXGI�t�@�N�g���[�̐���.
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(m_pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		// �X���b�v�`�F�C���̐ݒ�.
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = m_pWindow->GetWidth();
		desc.BufferDesc.Height = m_pWindow->GetHeight();
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.Format = m_BackBufferFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = FRAME_BUFFER_COUNT;
		desc.OutputWindow = m_pWindow->GetHwnd();
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// �X���b�v�`�F�C���̐���.
		ComPtr<IDXGISwapChain> pSwapChain;
		hr = m_pFactory->CreateSwapChain(m_pQueue.Get(), &desc, pSwapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// IDXGISwapChain4 ���擾.
		hr = pSwapChain.As(&m_pSwapChain);
		if (FAILED(hr))
		{
			return false;
		}

		// �o�b�N�o�b�t�@�ԍ����擾.
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// �s�v�ɂȂ����̂ŉ��.
		pSwapChain.Reset();
	}

	// �f�B�X�N���v�^�v�[���̐���.
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};

		desc.NodeMask = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 512;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_RES]))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = 256;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_SMP]))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = 512;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_RTV]))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = 512;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_DSV]))
		{
			return false;
		}
	}

	// �R�}���h���X�g�̐���.
	{
		if (!m_CommandList.Init(D3D12_COMMAND_LIST_TYPE_DIRECT, FRAME_BUFFER_COUNT))
		{
			return false;
		}
	}

	// �����_�[�^�[�Q�b�g�r���[�̐���.
	{
		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_ColorTarget[i].InitFromBackBuffer(m_pPool[POOL_TYPE_RTV], i, true))
			{
				return false;
			}
		}
	}

	// �[�x�X�e���V���o�b�t�@�̐���
	{
		if (!m_DepthTarget.Init(m_pPool[POOL_TYPE_DSV], nullptr, m_pWindow->GetWidth(), m_pWindow->GetHeight(), DXGI_FORMAT_D32_FLOAT, 1.0f, 0))
		{
			return false;
		}
	}

	// �t�F���X�̐���.
	if (!m_Fence.Init())
	{
		return false;
	}

	// �r���[�|�[�g�̐ݒ�.
	{
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = float(m_pWindow->GetWidth());
		m_Viewport.Height = float(m_pWindow->GetHeight());
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
	}

	// �V�U�[��`�̐ݒ�.
	{
		m_Scissor.left = 0;
		m_Scissor.right = m_pWindow->GetWidth();
		m_Scissor.top = 0;
		m_Scissor.bottom = m_pWindow->GetHeight();
	}

	// ����������
	return true;
}

void Renderer::Begin()
{
	// �R�}���h���X�g�̋L�^���J�n
	auto pCmd = m_CommandList.Reset();

	// ���\�[�X�o���A�̐ݒ�
	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// �����_�[�^�[�Q�b�g�r���[�̎擾
	auto rtv = m_ColorTarget[m_FrameIndex].GetHandleRTV();
	auto dsv = m_DepthTarget.GetHandleDSV();

	// �����_�[�^�[�Q�b�g�̐ݒ�
	pCmd->OMSetRenderTargets(1, &rtv->HandleCPU, FALSE, &dsv->HandleCPU);

	// �N���A�J���[
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	// �����_�[�^�[�Q�b�g�̃N���A
	m_ColorTarget[m_FrameIndex].ClearView(pCmd);
	// �[�x�X�e���V���̃N���A
	m_DepthTarget.ClearView(pCmd);

	// �ǂݍ��ݗp���\�[�X�o���A�ݒ�
//	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Renderer::End()
{
	// �R�}���h���X�g�̎擾
	auto pCmd = m_CommandList.Get();

	// �\���p���\�[�X�o���A�ݒ�
	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// �R�}���h���X�g�̋L�^���I��
	pCmd->Close();

	// �R�}���h���X�g�����s
	ID3D12CommandList* pLists[] = { pCmd };
	m_pQueue->ExecuteCommandLists(1, pLists);

	// ��ʂɕ\��
	Present(1);
}

void Renderer::TransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, UINT subresource, D3D12_RESOURCE_BARRIER_FLAGS flags)
{
	// �R�}���h���X�g�̎擾
	auto pCmd = m_CommandList.Get();

	// ���\�[�X�o���A�ݒ�
	CD3DX12_RESOURCE_BARRIER result = {};
	D3D12_RESOURCE_BARRIER& barrier = result;
	result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	result.Flags = flags;
	barrier.Transition.pResource = pResource;
	barrier.Transition.StateBefore = stateBefore;
	barrier.Transition.StateAfter = stateAfter;
	barrier.Transition.Subresource = subresource;

	// �o���A��ݒ�
	pCmd->ResourceBarrier(1, &barrier);
}

void Renderer::WaitGpu()
{
	// GPU�̏�������������܂őҋ@
	m_Fence.Wait(INFINITE);
}

void Renderer::Present(uint32_t interval)
{
	// ��ʂɕ\��.
	m_pSwapChain->Present(interval, 0);

	// �����҂�.
	m_Fence.Wait(INFINITE);

	// �t���[���ԍ����X�V.
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void Renderer::CheckSupportHDR()
{
	// ��������Ă��Ȃ��ꍇ�͏������Ȃ�.
	if (m_pSwapChain == nullptr || m_pFactory == nullptr || m_pDevice == nullptr)
	{
		return;
	}

	HRESULT hr = S_OK;

	// �E�B���h�E�̈���擾.
	RECT rect;
	GetWindowRect(m_pWindow->GetHwnd(), &rect);

	if (m_pFactory->IsCurrent() == false)
	{
		m_pFactory.Reset();
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(m_pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return;
		}
	}

	ComPtr<IDXGIAdapter1> pAdapter;
	hr = m_pFactory->EnumAdapters1(0, pAdapter.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}

	UINT i = 0;
	ComPtr<IDXGIOutput> currentOutput;
	ComPtr<IDXGIOutput> bestOutput;
	int bestIntersectArea = -1;

	// �e�f�B�X�v���C�𒲂ׂ�.
	while (pAdapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
	{
		auto ax1 = rect.left;
		auto ay1 = rect.top;
		auto ax2 = rect.right;
		auto ay2 = rect.bottom;

		// �f�B�X�v���C�̐ݒ���擾.
		DXGI_OUTPUT_DESC desc;
		hr = currentOutput->GetDesc(&desc);
		if (FAILED(hr))
		{
			return;
		}

		auto bx1 = desc.DesktopCoordinates.left;
		auto by1 = desc.DesktopCoordinates.top;
		auto bx2 = desc.DesktopCoordinates.right;
		auto by2 = desc.DesktopCoordinates.bottom;

		// �̈悪��v���邩�ǂ������ׂ�.
		int intersectArea = ComputeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
		if (intersectArea > bestIntersectArea)
		{
			bestOutput = currentOutput;
			bestIntersectArea = intersectArea;
		}

		i++;
	}

	// ��ԓK���Ă���f�B�X�v���C.
	ComPtr<IDXGIOutput6> pOutput6;
	hr = bestOutput.As(&pOutput6);
	if (FAILED(hr))
	{
		return;
	}

	// �o�͐ݒ���擾.
	DXGI_OUTPUT_DESC1 desc1;
	hr = pOutput6->GetDesc1(&desc1);
	if (FAILED(hr))
	{
		return;
	}

	// �F��Ԃ� ITU-R BT.2100 PQ���T�|�[�g���Ă��邩�ǂ����`�F�b�N.
	m_IsHDR = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
	m_MaxLuminance = desc1.MaxLuminance;
	m_MinLuminance = desc1.MinLuminance;
}

bool Renderer::IsSupportHDR() const
{
	return m_IsHDR;
}

float Renderer::GetMaxLuminance() const
{
	return m_MaxLuminance;
}

float Renderer::GetMinLuminance() const
{
	return m_MinLuminance;
}

void Renderer::Uninit()
{
	// GPU�����̊�����ҋ@.
	m_Fence.Sync();

	// �t�F���X�j��.
	m_Fence.Uninit();

	// �����_�[�^�[�Q�b�g�r���[�̔j��.
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_ColorTarget[i].Uninit();
	}

	// �[�x�X�e���V���r���[�̔j��.
	m_DepthTarget.Uninit();

	// �R�}���h���X�g�̔j��.
	m_CommandList.Uninit();

	for (auto i = 0; i < POOL_COUNT; ++i)
	{
		if (m_pPool[i] != nullptr)
		{
			m_pPool[i]->Release();
			m_pPool[i] = nullptr;
		}
	}

	// �X���b�v�`�F�C���̔j��.
	m_pSwapChain.Reset();

	// �R�}���h�L���[�̔j��.
	m_pQueue.Reset();

	// �f�o�C�X�̔j��.
	m_pDevice.Reset();

	m_pWindow = nullptr;
}

