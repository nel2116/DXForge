// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの実装
// 更新履歴
// 2024/05/25 作成
// 2024/06/24 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Renderer.h"
#include <System/d3dx12.h>

namespace
{
	void EnableDebugLayer()
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf()));

		// デバッグレイヤーの有効化
		if (SUCCEEDED(hr))
		{
			debug->EnableDebugLayer();
		}
	}

	// 交差領域の計算
	inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return std::max(0, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0, std::min(ay2, by2) - std::max(ay1, by1));
	}
}

bool Renderer::Init(Window* window)
{
	// ウィンドウクラスのポインタを設定
	m_pWindow = window;


#if defined(_DEBUG) || defined(DEBUG)
	// デバッグレイヤーの有効化
	EnableDebugLayer();
#endif	// _DEBUG || DEBUG

	// デバイスの生成.
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));
	if (FAILED(hr))
	{
		return false;
	}

	// コマンドキューの生成.
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

	// スワップチェインの生成.
	{
		// DXGIファクトリーの生成.
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(m_pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		// スワップチェインの設定.
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

		// スワップチェインの生成.
		ComPtr<IDXGISwapChain> pSwapChain;
		hr = m_pFactory->CreateSwapChain(m_pQueue.Get(), &desc, pSwapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// IDXGISwapChain4 を取得.
		hr = pSwapChain.As(&m_pSwapChain);
		if (FAILED(hr))
		{
			return false;
		}

		// バックバッファ番号を取得.
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// 不要になったので解放.
		pSwapChain.Reset();
	}

	// ディスクリプタプールの生成.
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

	// コマンドリストの生成.
	{
		if (!m_CommandList.Init(D3D12_COMMAND_LIST_TYPE_DIRECT, FRAME_BUFFER_COUNT))
		{
			return false;
		}
	}

	// レンダーターゲットビューの生成.
	{
		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_ColorTarget[i].InitFromBackBuffer(m_pPool[POOL_TYPE_RTV], i, true))
			{
				return false;
			}
		}
	}

	// 深度ステンシルバッファの生成
	{
		if (!m_DepthTarget.Init(m_pPool[POOL_TYPE_DSV], nullptr, m_pWindow->GetWidth(), m_pWindow->GetHeight(), DXGI_FORMAT_D32_FLOAT, 1.0f, 0))
		{
			return false;
		}
	}

	// フェンスの生成.
	if (!m_Fence.Init())
	{
		return false;
	}

	// ビューポートの設定.
	{
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = float(m_pWindow->GetWidth());
		m_Viewport.Height = float(m_pWindow->GetHeight());
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
	}

	// シザー矩形の設定.
	{
		m_Scissor.left = 0;
		m_Scissor.right = m_pWindow->GetWidth();
		m_Scissor.top = 0;
		m_Scissor.bottom = m_pWindow->GetHeight();
	}

	// 初期化成功
	return true;
}

void Renderer::Begin()
{
	// コマンドリストの記録を開始
	auto pCmd = m_CommandList.Reset();

	// リソースバリアの設定
	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// レンダーターゲットビューの取得
	auto rtv = m_ColorTarget[m_FrameIndex].GetHandleRTV();
	auto dsv = m_DepthTarget.GetHandleDSV();

	// レンダーターゲットの設定
	pCmd->OMSetRenderTargets(1, &rtv->HandleCPU, FALSE, &dsv->HandleCPU);

	// クリアカラー
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	// レンダーターゲットのクリア
	m_ColorTarget[m_FrameIndex].ClearView(pCmd);
	// 深度ステンシルのクリア
	m_DepthTarget.ClearView(pCmd);

	// 読み込み用リソースバリア設定
//	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Renderer::End()
{
	// コマンドリストの取得
	auto pCmd = m_CommandList.Get();

	// 表示用リソースバリア設定
	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストの記録を終了
	pCmd->Close();

	// コマンドリストを実行
	ID3D12CommandList* pLists[] = { pCmd };
	m_pQueue->ExecuteCommandLists(1, pLists);

	// 画面に表示
	Present(1);
}

void Renderer::TransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, UINT subresource, D3D12_RESOURCE_BARRIER_FLAGS flags)
{
	// コマンドリストの取得
	auto pCmd = m_CommandList.Get();

	// リソースバリア設定
	CD3DX12_RESOURCE_BARRIER result = {};
	D3D12_RESOURCE_BARRIER& barrier = result;
	result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	result.Flags = flags;
	barrier.Transition.pResource = pResource;
	barrier.Transition.StateBefore = stateBefore;
	barrier.Transition.StateAfter = stateAfter;
	barrier.Transition.Subresource = subresource;

	// バリアを設定
	pCmd->ResourceBarrier(1, &barrier);
}

void Renderer::WaitGpu()
{
	// GPUの処理が完了するまで待機
	m_Fence.Wait(INFINITE);
}

void Renderer::Present(uint32_t interval)
{
	// 画面に表示.
	m_pSwapChain->Present(interval, 0);

	// 完了待ち.
	m_Fence.Wait(INFINITE);

	// フレーム番号を更新.
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void Renderer::CheckSupportHDR()
{
	// 何も作られていない場合は処理しない.
	if (m_pSwapChain == nullptr || m_pFactory == nullptr || m_pDevice == nullptr)
	{
		return;
	}

	HRESULT hr = S_OK;

	// ウィンドウ領域を取得.
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

	// 各ディスプレイを調べる.
	while (pAdapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
	{
		auto ax1 = rect.left;
		auto ay1 = rect.top;
		auto ax2 = rect.right;
		auto ay2 = rect.bottom;

		// ディスプレイの設定を取得.
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

		// 領域が一致するかどうか調べる.
		int intersectArea = ComputeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
		if (intersectArea > bestIntersectArea)
		{
			bestOutput = currentOutput;
			bestIntersectArea = intersectArea;
		}

		i++;
	}

	// 一番適しているディスプレイ.
	ComPtr<IDXGIOutput6> pOutput6;
	hr = bestOutput.As(&pOutput6);
	if (FAILED(hr))
	{
		return;
	}

	// 出力設定を取得.
	DXGI_OUTPUT_DESC1 desc1;
	hr = pOutput6->GetDesc1(&desc1);
	if (FAILED(hr))
	{
		return;
	}

	// 色空間が ITU-R BT.2100 PQをサポートしているかどうかチェック.
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
	// GPU処理の完了を待機.
	m_Fence.Sync();

	// フェンス破棄.
	m_Fence.Uninit();

	// レンダーターゲットビューの破棄.
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_ColorTarget[i].Uninit();
	}

	// 深度ステンシルビューの破棄.
	m_DepthTarget.Uninit();

	// コマンドリストの破棄.
	m_CommandList.Uninit();

	for (auto i = 0; i < POOL_COUNT; ++i)
	{
		if (m_pPool[i] != nullptr)
		{
			m_pPool[i]->Release();
			m_pPool[i] = nullptr;
		}
	}

	// スワップチェインの破棄.
	m_pSwapChain.Reset();

	// コマンドキューの破棄.
	m_pQueue.Reset();

	// デバイスの破棄.
	m_pDevice.Reset();

	m_pWindow = nullptr;
}

