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


bool Renderer::Init(Window* window)
{
	// ウィンドウクラスのポインタを設定
	m_pWindow = window;


#if defined(_DEBUG) || defined(DEBUG)
	// デバッグレイヤーの有効化
	EnableDebugLayer();
#endif	// _DEBUG || DEBUG

	// --- D3D初期化 ---
	// 1.デバイスの生成
	if (!CreateDevice())
	{
		// デバイスの生成に失敗した場合
		assert(0 && "D3Dデバイスの生成に失敗しました");
		return false;
	}

	// 2.コマンドキューの生成
	if (!CreateCommandQueue())
	{
		// コマンドキューの生成に失敗した場合
		assert(0 && "コマンドキューの生成に失敗しました");
		return false;
	}

	// 3.スワップチェインの生成
	if (!CreateSwapChain())
	{
		// スワップチェインの生成に失敗した場合
		assert(0 && "スワップチェインの生成に失敗しました。");
		return false;
	}

	// 4.コマンドアロケータの生成
	if (!CreateCommandAllocator())
	{
		// コマンドアロケータの生成に失敗した場合
		assert(0 && "コマンドアロケータの生成に失敗しました。");
		return false;

	}

	// 5.コマンドリストの生成
	if (!CreateCommandList())
	{
		// コマンドリストの生成に失敗した場合
		assert(0 && "コマンドリストの生成に失敗しました。");
		return false;

	}

	// 6.RTVの生成
	if (!CreateRTV())
	{
		// RTVの生成に失敗した場合
		assert(0 && "RTVの生成に失敗しました。");
		return false;
	}

	// 7.フェンスの生成
	if (!CreateFence())
	{
		// フェンスの生成に失敗した場合
		assert(0 && "フェンスの生成に失敗しました。");
		return false;
	}

	// コマンドリストを閉じる
	m_pCmdList->Close();

	// 初期化成功
	return true;
}

void Renderer::Begin()
{
	// コマンドの記録を開始
	m_pCmdAllocator[m_FrameIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex].Get(), nullptr);

	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// リソースバリアの種類 : リソースの状態遷移
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// フラグ : 特になし
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();	// 
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	// 遷移前のリソースの状態 : プレゼント
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 遷移後のリソースの状態 : レンダーターゲット
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// サブリソース : 全て

	// リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	m_pCmdList->OMSetRenderTargets(1, &m_RTVHandle[m_FrameIndex], FALSE, nullptr);

	// クリアカラー
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	// レンダーターゲットビューをクリア
	m_pCmdList->ClearRenderTargetView(m_RTVHandle[m_FrameIndex], clearColor, 0, nullptr);

}

void Renderer::End()
{
	// リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// リソースバリアの種類 : リソースの状態遷移
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// フラグ :
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();	// リソース
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 遷移前のリソースの状態
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;	// 遷移後のリソースの状態
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// サブリソース

	// リソースバリア
	m_pCmdList->ResourceBarrier(1, &barrier);

	// コマンドの記録を終了
	m_pCmdList->Close();

	// コマンドの実行
	ID3D12CommandList* ppCmdLists[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(1, ppCmdLists);

	// 画面に表示
	Present(1);
}

void Renderer::Present(uint32_t interval)
{
	// スワップチェインの表示
	// 第1引数 : フレームの表示間隔 : 1で更新周期通りに表示、0で即時表示(2などでは2回目の垂直同期後に表示を同期 例:ディスプレイの更新周期が60Hzの場合、2で30FPSとなる)
	// 第2引数 : フラグ : 特になし
	m_pSwapChain->Present(interval, 0);

	// シグナル処理
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pCmdQueue->Signal(m_pFence.Get(), currentValue);

	// バックバッファ番号を更新
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 次のフレームの描画準備蒲田であれば待機する
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_fenceEvent);
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// 次のフレームのフェンスカウンターを増やす
	m_FenceCounter[m_FrameIndex]++;
}

void Renderer::Uninit()
{
	// GPUの処理完了を待つ
	WaitGpu();

	// イベント破棄
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

	// シグナル処理
	m_pCmdQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	// 完了時にイベントを設定する
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_fenceEvent);

	// イベントが設定されるまで待機
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// フェンスカウンターを増やす
	m_FenceCounter[m_FrameIndex]++;
}

bool Renderer::CreateDevice()
{
	// デバイスの生成
	// 第1引数 : ビデオアダプターへのポインタ。デフォルトのアダプターを使用する場合はnullptr
	// 第2引数 : デバイスの成功のために必要とされる最小機能レベル。D3D_FEATURE_LEVEL列挙型の値を指定
	// 第3引数 : デバイスを作成するためのインターフェースID
	// マクロを使用しない場合
	// auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), reinterpret_cast<void**>(&m_pDevice));
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return false; }
	return true;
}

bool Renderer::CreateCommandQueue()
{
	// コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// コマンドリストの種類 : GPUが実行可能なコマンドバッファを指定
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// コマンドキューの優先度 : 通常
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// コマンドキューのフラグ : 特になし
	desc.NodeMask = 0;	// ノードマスク : GUPが複数ある場合に使用

	// コマンドキューの生成
	auto hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return false; }
	return true;
}

bool Renderer::CreateSwapChain()
{
	// DXGIファクトリーの生成
	// DXGI : DirectXグラフィックスインストラクチャー
	// DXGIはカーネルモードドライバー及びシステムハードウェアとの通信を行うためのAPI
	// DXGI自体はDirect3Dに依存しない
	// 基本的にはどのようなGPUが接続されているかの列挙や、描画したフレームを画面へ表示するために使う
	IDXGIFactory4* pFactory = nullptr;	// DXGIファクトリーインターフェース
	auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	// エラーチェック
	if (FAILED(hr)) { return false; }

	// スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferDesc.Width = m_pWindow->GetWidth();			// 解像度の横幅を指定
	desc.BufferDesc.Height = m_pWindow->GetHeight();		// 解像度の縦幅を指定
	desc.BufferDesc.RefreshRate.Numerator = 60;				// リフレッシュレートの分子
	desc.BufferDesc.RefreshRate.Denominator = 1;			// リフレッシュレートの分母
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// スキャンラインの順序 : 特に指定しない
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					// スケーリング : 特に指定しない
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// フォーマット	: RGBA8bit
	desc.SampleDesc.Count = 1;								// サンプル数
	desc.SampleDesc.Quality = 0;							// サンプル品質
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// バッファの使用法 : 出力レンダーターゲットとして使用
	desc.BufferCount = FRAME_BUFFER_COUNT;					// バッファ数
	desc.OutputWindow = m_pWindow->GetHwnd();				// 出力先のウィンドウハンドル
	desc.Windowed = TRUE;									// ウィンドウモード
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// スワップエフェクト : バックバッファの内容を速やかに破棄
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// フラグ : モード切り替えを許可

	// スワップチェインの生成
	IDXGISwapChain* pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(m_pCmdQueue.Get(), &desc, &pSwapChain);
	// エラーチェック
	if (FAILED(hr))
	{
		SAFE_RELEASE(pFactory);
		return false;
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr))
	{
		SAFE_RELEASE(pFactory);
		SAFE_RELEASE(pSwapChain);
		return false;
	}

	// バックバッファ番号を取得
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// 不要になったので解放
	SAFE_RELEASE(pFactory);
	SAFE_RELEASE(pSwapChain);

	return true;
}

bool Renderer::CreateCommandAllocator()
{
	// コマンドアロケータの生成
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		// 第1引数 : コマンドリストの種類 : GPUが実行可能なコマンドバッファを指定
		auto hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCmdAllocator[i].ReleaseAndGetAddressOf()));
		// エラーチェック
		if (FAILED(hr)) { return false; }
	}
	return true;
}

bool Renderer::CreateCommandList()
{
	// コマンドリストの生成
	// 第1引数 : 複数のGPUノードがある場合に、ノードを識別するためのビットマスク : GPUが1つしかない場合は0
	// 第2引数 : コマンドリストの種類 : GPUが実行可能なコマンドバッファを指定
	// 第3引数 : コマンドアロケータ : コマンドリストが使用するコマンドアロケータ
	// 第4引数 : パイプラインステート : 後で設定するためにnullptrを指定
	// 第5引数 : インターフェースID
	auto hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAllocator[m_FrameIndex].Get(), nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return false; }
	return true;
}

bool Renderer::CreateRTV()
{
	// ====== レンダーターゲットビュー(RTV) ======
	// レンダーターゲットはDirect3Dの描画先
	// 実体はバックバッファやテクスチャといったリソース
	// リソースは単にメモリ領域を持つオブジェクトがわかっているだけでGPUのアドレスがどこになるのかわからない
	// また、バッファ要領はわかっているが、バッファの区切り方によって2次元のテクスチャとして使うのか、3次元のテクスチャとして使うのかわからない
	// そこで、リソースをどのように使うのかを指定するのがビュー
	// RTVの作成方法
	// 1,リソースの生成
	// 2,ディスクリプタヒープの生成
	// 3,レンダーターゲットビューの生成
	// 今回の場合はスワップチェインのバックバッファをリソースとするため、1は不要
	// 次にやるべきことはディスクリプタのアドレス取得

	// ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FRAME_BUFFER_COUNT;	// ディスクリプタ数 : バッファ数
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// ヒープの種類	: RTV
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// フラグ : 特になし
	desc.NodeMask = 0;	// ノードマスク : GPUが複数ある場合に使用

	// ディスクリプタヒープの生成
	auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pRTVHeap.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return false; }

	// レンダーターゲットビューの生成
	auto handle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();
	// この関数で取得できる値は固定値だが、デバイス依存のため必ず取得する
	auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].ReleaseAndGetAddressOf()));
		// エラーチェック
		if (FAILED(hr)) { return false; }

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// フォーマット : sRGBでのRGBA8bit
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// ビューの次元 : 2D
		viewDesc.Texture2D.MipSlice = 0;	// ミップマップスライス : 0
		viewDesc.Texture2D.PlaneSlice = 0;	// プレーンスライス : 0

		m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);
		m_RTVHandle[i] = handle;
		handle.ptr += incrementSize;
	}

	return true;
}

bool Renderer::CreateFence()
{
	// CPUとGPUの同期処理はApplication側の責務
	// フェンスはCPUとGPUの同期を取るための仕組み
	// フェンスの値がインクリメントされたかどうかでGPUの処理が終わったかどうかを判断する

	// フェンスカウンターをリセット
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_FenceCounter[i] = 0;
	}

	// フェンスの生成
	// 第1引数 : フェンスの初期値
	// 第2引数 : フェンスのフラグ : 特になし
	// 第3引数 : インターフェースID
	auto hr = m_pDevice->CreateFence(m_FenceCounter[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return false; }

	m_FenceCounter[m_FrameIndex]++;

	// フェンスイベントの生成
	// 描画処理が実際に終わるまで待つためOSのイベントを使用
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	// エラーチェック
	if (m_fenceEvent == nullptr) { return false; }

	return true;
}
