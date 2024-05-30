// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの実装
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Renderer.h"
// ====== 名前空間 ======
using Microsoft::WRL::ComPtr;

// ====== メンバ関数 ======

namespace
{
	// デバッグレイヤーをオンに
	void EnableDebugLayer()
	{
		ComPtr<ID3D12Debug> pDebugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.ReleaseAndGetAddressOf()))))
		{
			pDebugController->EnableDebugLayer();
		}
	}
}

// レンダラーの初期化
bool Renderer::Init(Window* window)
{
	SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#ifdef _DEBUG
	EnableDebugLayer();
	SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
#endif // _DEBUG

	// ウィンドウの取得
	m_pWindow = window;
	if (!CreateFactory())
	{
		assert(0 && "ファクトリーの作成に失敗しました。");
		return false;
	}
	if (!CreateDevice())
	{
		assert(0 && "D3D12デバイスの作成に失敗しました。");
		return false;
	}
	if (!CreateCommandList())
	{
		assert(0 && "コマンドリストの作成に失敗しました。");
		return false;
	}
	if (!CreateSwapChain())
	{
		assert(0 && "スワップチェインの作成に失敗しました。");
		return false;
	}

	m_pRTVHeap = make_unique<RTVHeap>();
	if (!m_pRTVHeap->Create(this, HeapType::RTV, 100))
	{
		assert(0 && "RTVヒープの作成に失敗しました。");
		return false;
	}
	m_upCBVSRVUAVHeap = make_unique<CBVSRVUAVHeap>();
	if (!m_upCBVSRVUAVHeap->Create(this, HeapType::CBVSRVUAV, DirectX::XMFLOAT3(100, 100, 100)))
	{
		assert(0 && "CBVSRVUAVヒープの作成に失敗しました。");
		return false;
	}
	m_upCBufferAllocater = make_unique<CBufferAllocater>();
	m_upCBufferAllocater->Create(this, m_upCBVSRVUAVHeap.get());

	m_upDSVHeap = make_unique<DSVHeap>();
	if (!m_upDSVHeap->Create(this, HeapType::DSV, 100))
	{
		assert(0 && "DSVヒープの作成に失敗しました。");
		return false;
	}

	m_upDepthStencil = make_unique<DepthStencil>();
	if (!m_upDepthStencil->Create(this, XMFLOAT2(WINDOW_WIDTH, WINDOW_HEIGHT)))
	{
		assert(0 && "デプスステンシルの作成に失敗しました。");
		return false;
	}

	if (!CreateSwapChainRTV())
	{
		assert(0 && "スワップチェインのRTVの作成に失敗しました。");
		return false;
	}

	if (!CreateFence())
	{
		assert(0 && "フェンスの作成に失敗しました。");
		return false;
	}

	return true;
}

void Renderer::Uninit()
{
	// フェンスの待機
	WaitForCmdQueue();

	m_pWindow = nullptr;
}

void Renderer::Begin3DDraw()
{
	// リソースバリアのステートをレンダーターゲットに変更
	auto bbIdx = m_pSwapChain->GetCurrentBackBufferIndex();
	TransBarrier(m_pBackBuffers[bbIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// レンダーターゲットをセット
	auto rtvH = m_pRTVHeap->GetCPUHandle(bbIdx);
	auto dsvH = m_upDSVHeap->GetCPUHandle(m_upDepthStencil->GetDSVNumber());
	m_pCmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// レンダーターゲットのクリア
	m_pCmdList->ClearRenderTargetView(rtvH, m_clearColor, 0, nullptr);

	// デプスバッファのクリア
	m_upDepthStencil->ClearBuffer();
}

void Renderer::Begin2DDraw()
{
	// リソースバリアのステートをレンダーターゲットに変更
	auto bbIdx = m_pSwapChain->GetCurrentBackBufferIndex();

	// レンダーターゲットをセット
	auto rtvH = m_pRTVHeap->GetCPUHandle(bbIdx);
	m_pCmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);
}

void Renderer::EndDraw()
{
	auto bbIdx = m_pSwapChain->GetCurrentBackBufferIndex();

	// リソースバリアのステートをプレゼントに変更
	TransBarrier(m_pBackBuffers[bbIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストを閉じて実行する
	m_pCmdList->Close();
	ID3D12CommandList* cmdLists[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(1, cmdLists);

	// コマンドリストの同期を待つ
	WaitForCmdQueue();

	// コマンドアロケータとコマンドリストのリセット
	m_pCmdAllocator->Reset();
	m_pCmdList->Reset(m_pCmdAllocator.Get(), nullptr);

	// スワップチェインにプレゼント
	m_pSwapChain->Present(1, 0);
}

void Renderer::WaitForCmdQueue()
{
	m_pCmdQueue->Signal(m_pFence.Get(), ++m_fanceVal);
	if (m_pFence->GetCompletedValue() < m_fanceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	// イベントハンドルの取得
		if (!event)
		{
			assert(0 && "イベントエラー、アプリを終了します。");
		}
		m_pFence->SetEventOnCompletion(m_fanceVal, event);	// イベントの設定
		WaitForSingleObject(event, INFINITE);	// イベントが発生するまで待機
		CloseHandle(event);	// イベントハンドルの解放
	}
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
	m_clearColor[0] = r;
	m_clearColor[1] = g;
	m_clearColor[2] = b;
	m_clearColor[3] = a;
}

// ファクトリーの作成
bool Renderer::CreateFactory()
{
	UINT flagsDXGI = 0;

#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	auto hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_pFactory.ReleaseAndGetAddressOf()));

	if (FAILED(hr))
	{
		// エラーメッセージの表示
		MessageBox(nullptr, "DXGIファクトリーの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	return true;
}

// デバイスの作成
bool Renderer::CreateDevice()
{
	// ------ 変数宣言 ------
	ComPtr<IDXGIAdapter> pSelectAdapter = nullptr;
	vector<ComPtr<IDXGIAdapter>> pAdapters;
	vector<DXGI_ADAPTER_DESC> descs;

	// ------ アダプターの列挙 ------
	for (UINT idx = 0; 1; ++idx)
	{
		pAdapters.push_back(nullptr);
		HRESULT hr = m_pFactory->EnumAdapters(idx, &pAdapters[idx]);

		if (hr == DXGI_ERROR_NOT_FOUND) { break; }

		descs.push_back({});
		pAdapters[idx]->GetDesc(&descs[idx]);
	}

	// ------ 選択 ------
	E_GPU_TIER tier = E_GPU_TIER::Kind;
	// 優先度の高いGPUドライバーを選択
	for (int i = 0; i < descs.size(); ++i)
	{
		if (wstring(descs[i].Description).find(L"NVIDIA") != wstring::npos)
		{
			pSelectAdapter = pAdapters[i];
			break;
		}
		else if (wstring(descs[i].Description).find(L"Amd") != wstring::npos)
		{
			if (tier > E_GPU_TIER::Amd)
			{
				pSelectAdapter = pAdapters[i];
				tier = E_GPU_TIER::Amd;
			}
		}
		else if (wstring(descs[i].Description).find(L"Intel") != wstring::npos)
		{
			if (tier > E_GPU_TIER::Intel)
			{
				pSelectAdapter = pAdapters[i];
				tier = E_GPU_TIER::Intel;
			}
		}
		else if (wstring(descs[i].Description).find(L"Qualcomm") != wstring::npos)
		{
			if (tier > E_GPU_TIER::Qualcomm)
			{
				pSelectAdapter = pAdapters[i];
				tier = E_GPU_TIER::Qualcomm;
			}
		}
		else if (wstring(descs[i].Description).find(L"Arm") != wstring::npos)
		{
			if (tier > E_GPU_TIER::Arm)
			{
				pSelectAdapter = pAdapters[i];
				tier = E_GPU_TIER::Arm;
			}
		}
	}

	// ------ デバイスの作成 ------
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	for (auto lv : levels)
	{
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf())) == S_OK)
		{
			m_featureLevel = lv;
			break;
		}
	}

	return true;
}

// コマンドリストの作成
bool Renderer::CreateCommandList()
{
	// コマンドアロケータの作成
	auto hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドアロケータの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	m_pCmdAllocator->SetName(L"CmdAllocator");

	// コマンドリストの作成
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドリストの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	m_pCmdList->SetName(L"CmdList");

	// コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// タイムアウトなし
	desc.NodeMask = 0;										// アダプターを一つしか使わないときは0
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// 特に指定なし
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;				// コマンドリストと合わせる

	hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "コマンドキューの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	m_pCmdQueue->SetName(L"CmdQueue");

	return true;
}

// スワップチェインの作成
bool Renderer::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = m_pWindow->GetWidth();
	desc.Height = m_pWindow->GetHeight();
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	desc.BufferCount = 2;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// フリップ後は速やかに破棄
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ウィンドウとフルスクリーン切り替えを許可

	auto hr = m_pFactory->CreateSwapChainForHwnd(m_pCmdQueue.Get(), m_pWindow->GetHwnd(), &desc, nullptr, nullptr, (IDXGISwapChain1**)m_pSwapChain.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "スワップチェインの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::CreateSwapChainRTV()
{
	for (int i = 0; i < (int)m_pBackBuffers.size(); ++i)
	{
		auto hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pBackBuffers[i].ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			MessageBox(nullptr, "スワップチェインのバックバッファの取得に失敗しました。", "エラー", MB_OK);
			return false;
		}
		m_pRTVHeap->CreateRTV(m_pBackBuffers[i].Get());
	}
	m_pBackBuffers[0]->SetName(L"BackBuffer0");
	m_pBackBuffers[1]->SetName(L"BackBuffer1");
	return true;
}

bool Renderer::CreateFence()
{
	auto hr = m_pDevice->CreateFence(m_fanceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "フェンスの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	return true;
}

void Renderer::TransBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pResource;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = 0;

	m_pCmdList->ResourceBarrier(1, &barrier);
}
