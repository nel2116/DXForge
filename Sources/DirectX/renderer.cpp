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
	// 解放処理

	// 行列の解放
	m_constBuff->Unmap(0, nullptr);
	// 定数バッファの解放
	SAFE_RELEASE(m_constBuff);
	// テストテクスチャの解放
	SAFE_RELEASE(m_pBasicDescHeap);
	// テクスチャバッファの解放
	SAFE_RELEASE(m_texBuff);
	// ルートシグネチャの解放
	SAFE_RELEASE(m_rootSignature);
	// パイプラインステートの解放
	SAFE_RELEASE(m_pipelineState);
	// VS, PSの解放
	SAFE_RELEASE(m_vsBlob);
	SAFE_RELEASE(m_psBlob);
	// 頂点バッファの解放
	SAFE_RELEASE(m_vertexBuff);
	// インデックスバッファの解放
	SAFE_RELEASE(m_indexBuff);
	// バックバッファの解放
	for (auto bb : m_backBuffers)
	{
		SAFE_RELEASE(bb);
	}
	// フェンスの解放
	SAFE_RELEASE(m_fence);
	// レンダーターゲットビューの解放
	SAFE_RELEASE(m_pRTV);
	// コマンドキューの解放
	SAFE_RELEASE(m_cmdQueue);
	// コマンドアロケータの解放
	SAFE_RELEASE(m_cmdAllocator);
	// コマンドリストの解放
	SAFE_RELEASE(m_cmdList);
	// スワップチェインの解放
	SAFE_RELEASE(m_swapChain);
	// DXGIファクトリの解放
	SAFE_RELEASE(m_dxgiFactory);
	// デバイスの解放
	SAFE_RELEASE(m_dev);
}

bool Renderer::Init()
{
#ifdef _DEBUG

	// デバッグレイヤーの有効化
	// デバイス生成前にやっておかないと、デバイスがロストしてしまう
	EnableDebugLayer();

#endif // _DEBUG

	// ----- DirectX12の初期化 -----

	// デバイスの作成
	if (!CreateDevice()) { return false; }
	// コマンドリストの作成
	if (!CreateCommandList()) { return false; }
	// スワップチェインの作成
	if (!CreateSwapChain()) { return false; }
	// レンダーターゲットビューの作成
	if (!CreateRenderTargetView()) { return false; }
	// フェンスの作成
	if (!CreateFence()) { return false; }
	// 頂点バッファの作成
	if (!CreateVertexBuffer()) { return false; }
	// シェーダーの作成
	if (!CreateShader()) { return false; }
	// インプットレイアウトの作成
	if (!CreateInputLayout()) { return false; }
	// ルートシグネチャの作成
	if (!CreateRootSignature()) { return false; }
	// パイプラインステートの作成
	if (!CreatePipelineState()) { return false; }
	// ビューポートの作成
	if (!CreateViewport()) { return false; }
	// テストテクスチャの作成
	// if (!CreateTestTexture()) { return false; }
	// モデルの読み込み
	if (!LoadModel()) { return false; }
	// テクスチャの作成
	if (!CreateTexture()) { return false; }
	// 定数バッファの作成
	if (!CreateConstantBuffer()) { return false; }
	// シェーダーリソースビューの作成
	if (!CreateShaderResourceView()) { return false; }

	// 正常に初期化できた
	return true;
}

void Renderer::Update()
{
	//// 定数バッファの設定
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
	//// ワールドビュープロジェクション行列
	//XMMATRIX wvp = world * view * proj;
	//*m_pMapMat = wvp;
}

void Renderer::Draw()
{
	// バックバッファのインデックス
	UINT bbIdx = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	                    // リソースの状態遷移
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		                    // 特に指定なし
	barrier.Transition.pResource = m_backBuffers[bbIdx];	                    // リソース
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// サブリソース
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	            // 
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	        //
	m_cmdList->ResourceBarrier(1, &barrier);	                                // リソースバリアの設定

	// パイプラインステートの設定
	m_cmdList->SetPipelineState(m_pipelineState);	                        // パイプラインステートの設定

	// レンダーターゲットを指定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * bbIdx;
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// 画面クリア
	float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// 白色
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);


	// ----- 描画命令を挟む -----
	m_cmdList->RSSetViewports(1, &m_viewport);	                            // ビューポートの設定
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);	                    // シザリング矩形の設定
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);	                // ルートシグネチャの設定
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジの設定
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBuffView);	                // 頂点バッファの設定
	m_cmdList->IASetIndexBuffer(&m_indexBuffView);	                        // インデックスバッファの設定
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);	                // ルートシグネチャの設定
	m_cmdList->SetDescriptorHeaps(1, &m_pBasicDescHeap);	                    // ディスクリプタヒープの設定
	m_cmdList->SetGraphicsRootDescriptorTable(
		0,																		// ルートパラメーターインデックス 
		m_pBasicDescHeap->GetGPUDescriptorHandleForHeapStart());				// ヒープアドレス
	m_cmdList->DrawInstanced(vertNum, 1, 0, 0);								// 描画命令

	// レンダーターゲットの状態遷移
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;	        //
	m_cmdList->ResourceBarrier(1, &barrier);

	// コマンドリストのクローズ
	m_cmdList->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { m_cmdList };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	// 待ち
	m_cmdQueue->Signal(m_fence, ++m_fenceVal);
	if (m_fence->GetCompletedValue() < m_fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// キューをクリア
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator, nullptr);	// 再びコマンドリストをためる準備

	// バッファの切り替え
	m_swapChain->Present(1, 0);
}

void Renderer::EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	if (SUCCEEDED(hr))
	{	// 正常にデバッグレイヤーが取得できた
		debugLayer->EnableDebugLayer();	// デバッグレイヤーを有効にする
		debugLayer->Release();			// インターフェースを解放
	}
	else
	{	// デバッグレイヤーが取得できなかった
		MessageBox(nullptr, "D3D12GetDebugInterface Failed.", "Error", MB_OK);
	}
}

bool Renderer::CreateDevice()
{
	// DXGIファクトリの生成
	HRESULT result = S_OK;
	if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory))))
	{
		if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory))))
		{
			MessageBox(nullptr, "CreateDXGIFactory2 Failed.", "Error", MB_OK);
			return false;
		}
	}

	// アダプターの列挙
	IDXGIAdapter1* adapter = nullptr;
	vector<IDXGIAdapter1*> adapters;
	for (int i = 0; m_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(adapter);
	}
	// アダプターを識別するための情報を取得
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC1 desc;
		adpt->GetDesc1(&desc);
		wstring strDesc = desc.Description;	// アダプターの説明
		// 探したいアダプターの説明を指定
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			adapter = adpt;
			break;
		}
	}

	// デバイスの生成
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

	// エラーチェック
	if (FAILED(result))
	{
		MessageBox(nullptr, "D3D12CreateDevice Failed.", "Error", MB_OK);
		return false;
	}

	// アダプターの解放
	for (auto adpt : adapters)
	{
		SAFE_RELEASE(adpt);
	}

	return true;
}

bool Renderer::CreateCommandList()
{
	// コマンドアロケータの生成
	auto hr = m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommandAllocator Failed.", "Error", MB_OK);
		return false;
	}

	// コマンドリストの生成
	hr = m_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator, nullptr, IID_PPV_ARGS(&m_cmdList));

	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommandList Failed.", "Error", MB_OK);
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	        // タイムアウトなし
	queueDesc.NodeMask = 0;	                                    // アダプターは一つしか使わないときは0
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// プライオリティは特に指定なし
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	        // コマンドリストと合わせる

	// コマンドキューの生成
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
	swapChainDesc.Width = WINDOW_WIDTH;					            // ウィンドウの幅
	swapChainDesc.Height = WINDOW_HEIGHT;				            // ウィンドウの高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	            // ピクセルフォーマット
	swapChainDesc.Stereo = false;						            // ステレオ表示
	swapChainDesc.SampleDesc.Count = 1;					            // マルチサンプリングの設定
	swapChainDesc.SampleDesc.Quality = 0;				            // マルチサンプリングの品質
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	            // バッファの使用法
	swapChainDesc.BufferCount = 2;						            // バッファの数
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;		            // バックバッファはウィンドウサイズに合わせて拡大縮小
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	    // フリップ後はバックバッファを破棄
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;	        // アルファモード
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ウィンドウモードとフルスクリーンモードの切り替えを許可

	// スワップチェインの生成
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

	// ディスクリプタヒープの設定
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	    // レンダーターゲットビュー
	heapDesc.NodeMask = 0;							    // ノードマスク
	heapDesc.NumDescriptors = 2;					    // ディスクリプタの数, バッファの数と同じ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// 特に指定なし

	// ディスクリプタヒープの生成
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

	// レンダーターゲットビューの作成
	// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	// // SRGB レンダーターゲットビュー設定
	// D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	// rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// SRGB フォーマット(ガンマ補正あり)
	// rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2D テクスチャ

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		hr = m_swapChain->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(&m_backBuffers[idx]));
		if (FAILED(hr))
		{
			MessageBox(nullptr, "GetBuffer Failed.", "Error", MB_OK);
			return false;
		}
		m_dev->CreateRenderTargetView(m_backBuffers[idx], nullptr, handle); // ガンマ補正をかけたいときは第二引数にrtvDescを指定
		handle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	return true;
}

bool Renderer::CreateFence()
{
	// フェンスの生成
	auto hr = m_dev->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

	return true;
}

bool Renderer::CreateVertexBuffer()
{
	// 頂点情報の設定
	//Vertex vertices[] =
	//{
	//	{ { -1.0f, -1.0f, 0.0f}, { 0.0f, 1.0f } },	// 左下
	//	{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },	// 左上
	//	{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },	// 右下
	//	{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },	// 右上
	//};

	// バッファの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	                    // アップロードヒープ
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの設定

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元
	resDesc.Width = vertices.size();	                    // 頂点が入るだけのサイズ
	resDesc.Height = 1;	                                    // 高さ
	resDesc.DepthOrArraySize = 1;	                        // デプス
	resDesc.MipLevels = 1;	                                // ミップマップ
	resDesc.Format = DXGI_FORMAT_UNKNOWN;	                // フォーマット
	resDesc.SampleDesc.Count = 1;	                        // サンプル数
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	            // フラグ
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	    // テクスチャのレイアウト

	// リソースの生成
	auto hr = m_dev->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuff));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// バッファビューの設定
	m_vertexBuffView.BufferLocation = m_vertexBuff->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_vertexBuffView.SizeInBytes = vertices.size();	                    // 全バイト数
	m_vertexBuffView.StrideInBytes = pmdvertex_size;	                // 1頂点あたりのバイト数

	// バッファにデータをコピー
	unsigned char* vertMap = nullptr;
	hr = m_vertexBuff->Map(0, nullptr, (void**)&vertMap);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "VertexMap Failed.", "Error", MB_OK);
		return false;
	}
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	m_vertexBuff->Unmap(0, nullptr);

	// インデックスの設定
	unsigned short indices[6] = { 0, 1, 2, 2, 1, 3 };

	// 設定は、バッファサイズ以外は頂点バッファと同じ
	resDesc.Width = sizeof(indices);	// インデックスが入るだけのサイズ

	// リソースの生成
	hr = m_dev->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_indexBuff));
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// バッファにデータをコピー
	unsigned short* mappedIdx = nullptr;
	m_indexBuff->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	m_indexBuff->Unmap(0, nullptr);

	// バッファビューの設定
	m_indexBuffView.BufferLocation = m_indexBuff->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_indexBuffView.Format = DXGI_FORMAT_R16_UINT;	                        // インデックスのフォーマット
	m_indexBuffView.SizeInBytes = sizeof(indices);	                        // バッファのサイズ

	return true;
}

bool Renderer::CreateShader()
{
	// シェーダーのコンパイル
	ID3DBlob* errorBlob = nullptr;

	// VSのコンパイル
	auto hr = D3DCompileFromFile(
		L"Sources/HLSL/VertexShader/VS_Basic.hlsl",	        // シェーダーファイル
		nullptr,			                                // マクロはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	                // インクルードファイルの指定,今回は標準
		"VS_Main", "vs_5_0",	                            // エントリーポイントとバージョン
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// デバッグ用、最適化はスキップ
		0,	                                                // フラグ
		&m_vsBlob,	                                        // コンパイル後のデータ
		&errorBlob	                                        // エラーメッセージ
	);
	if (FAILED(hr))
	{
		// エラーメッセージの表示
		string errStr;	// 受取用の文字列
		errStr.resize(errorBlob->GetBufferSize());	// エラーメッセージのサイズを取得

		// データコピー
		copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());
		errStr += "\n";

		// エラーメッセージの表示
		MessageBoxA(nullptr, errStr.c_str(), "Error", MB_OK);
		OutputDebugStringA(errStr.c_str());
		errorBlob->Release();

		return false;
	}

	// PSのコンパイル
	hr = D3DCompileFromFile(
		L"Sources/HLSL/PixelShader/PS_Basic.hlsl",	        // シェーダーファイル
		nullptr,			                                // マクロはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	                // インクルードファイルの指定,今回は標準
		"PS_Main", "ps_5_0",	                            // エントリーポイントとバージョン
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// デバッグ用、最適化はスキップ
		0,	                                                // フラグ
		&m_psBlob,	                                        // コンパイル後のデータ
		&errorBlob	                                        // エラーメッセージ
	);
	if (FAILED(hr))
	{
		// エラーメッセージの表示
		string errStr;	// 受取用の文字列
		errStr.resize(errorBlob->GetBufferSize());	// エラーメッセージのサイズを取得

		// データコピー
		copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());
		errStr += "\n";

		// エラーメッセージの表示
		MessageBoxA(nullptr, errStr.c_str(), "Error", MB_OK);
		OutputDebugStringA(errStr.c_str());
		errorBlob->Release();

		return false;
	}
	return true;
}

bool Renderer::CreateInputLayout()
{
	// インプットレイアウトの設定
	m_inputLayout[0].SemanticName = "POSITION";	                                    // セマンティック名
	m_inputLayout[0].SemanticIndex = 0;	                                            // セマンティックインデックス
	m_inputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	                        // フォーマット
	m_inputLayout[0].InputSlot = 0;	                                                // 入力スロット
	m_inputLayout[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // オフセット
	m_inputLayout[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 入力スロットのクラス
	m_inputLayout[0].InstanceDataStepRate = 0;	                                    // インスタンスデータのステップレート

	m_inputLayout[1].SemanticName = "NORMAL";	                                    // セマンティック名
	m_inputLayout[1].SemanticIndex = 0;	                                            // セマンティックインデックス
	m_inputLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;                          // フォーマット
	m_inputLayout[1].InputSlot = 0;	                                                // 入力スロット
	m_inputLayout[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // オフセット
	m_inputLayout[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 入力スロットのクラス
	m_inputLayout[1].InstanceDataStepRate = 0;	                                    // インスタンスデータのステップレート

	m_inputLayout[2].SemanticName = "TEXCCORD";	                                    // セマンティック名
	m_inputLayout[2].SemanticIndex = 0;	                                            // セマンティックインデックス
	m_inputLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;		                        // フォーマット
	m_inputLayout[2].InputSlot = 0;	                                                // 入力スロット
	m_inputLayout[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // オフセット
	m_inputLayout[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 入力スロットのクラス
	m_inputLayout[2].InstanceDataStepRate = 0;	                                    // インスタンスデータのステップレート

	m_inputLayout[3].SemanticName = "BONE_NO";	                                    // セマンティック名
	m_inputLayout[3].SemanticIndex = 0;	                                            // セマンティックインデックス
	m_inputLayout[3].Format = DXGI_FORMAT_R16G16_UINT;		                        // フォーマット
	m_inputLayout[3].InputSlot = 0;	                                                // 入力スロット
	m_inputLayout[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // オフセット
	m_inputLayout[3].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 入力スロットのクラス
	m_inputLayout[3].InstanceDataStepRate = 0;	                                    // インスタンスデータのステップレート

	m_inputLayout[4].SemanticName = "WEIGHT";	                                    // セマンティック名
	m_inputLayout[4].SemanticIndex = 0;	                                            // セマンティックインデックス
	m_inputLayout[4].Format = DXGI_FORMAT_R8_UINT;			                        // フォーマット
	m_inputLayout[4].InputSlot = 0;	                                                // 入力スロット
	m_inputLayout[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // オフセット
	m_inputLayout[4].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 入力スロットのクラス
	m_inputLayout[4].InstanceDataStepRate = 0;	                                    // インスタンスデータのステップレート

	m_inputLayout[5].SemanticName = "EDGE_FLG";	                                    // セマンティック名
	m_inputLayout[5].SemanticIndex = 0;	                                            // セマンティックインデックス
	m_inputLayout[5].Format = DXGI_FORMAT_R8_UINT;			                        // フォーマット
	m_inputLayout[5].InputSlot = 0;	                                                // 入力スロット
	m_inputLayout[5].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	            // オフセット
	m_inputLayout[5].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 入力スロットのクラス
	m_inputLayout[5].InstanceDataStepRate = 0;	                                    // インスタンスデータのステップレート

	return true;
}

bool Renderer::CreateRootSignature()
{
	// テクスチャ用レジスター0番
	D3D12_DESCRIPTOR_RANGE descRange[2] = {};
	descRange[0].NumDescriptors = 1;	// テクスチャ一つ
	descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// 種別はテクスチャ
	descRange[0].BaseShaderRegister = 0;	// 0番スロットから
	descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// オフセットは追加

	// 定数バッファ用レジスター0番
	descRange[1].NumDescriptors = 1;	// 定数バッファ一つ
	descRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// 種別は定数バッファ
	descRange[1].BaseShaderRegister = 0;	// 0番スロットから
	descRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// オフセットは追加

	// ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// ディスクリプタテーブル
	// 配列先頭アドレス
	rootParam.DescriptorTable.pDescriptorRanges = &descRange[0];
	// ディスクリプタレンジ数
	rootParam.DescriptorTable.NumDescriptorRanges = 2;
	// すべてのシェーダーから見える
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					// 横方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					// 縦方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					// 奥行きの繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// ボーダーは黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					// 線形補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									// ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												// ミップマップ最小値
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			// ピクセルシェーダーから見える
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;				// リサンプリングしない

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 入力アセンブラ入力レイアウトを許可

	// ルートパラメータの設定
	rootSigDesc.pParameters = &rootParam;	// ルートパラメータの先頭アドレス
	rootSigDesc.NumParameters = 1;			// ルートパラメータの数

	// サンプラーの設定
	rootSigDesc.pStaticSamplers = &samplerDesc;	// サンプラーの設定
	rootSigDesc.NumStaticSamplers = 1;			// サンプラーの数

	// バイナリコードの作成
	ID3DBlob* errorBlob = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	auto hr = D3D12SerializeRootSignature(
		&rootSigDesc,					// ルートシグネチャの設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ルートシグネチャバージョン
		&signatureBlob,				// バイナリデータ
		&errorBlob						// エラーメッセージ
	);
	if (FAILED(hr))
	{
		// エラーメッセージの表示
		string errStr;	// 受取用の文字列
		errStr.resize(errorBlob->GetBufferSize());	// エラーメッセージのサイズを取得

		// データコピー
		copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());
		errStr += "\n";

		// エラーメッセージの表示
		MessageBoxA(nullptr, errStr.c_str(), "Error", MB_OK);
		OutputDebugStringA(errStr.c_str());
		errorBlob->Release();

		return false;
	}

	// ルートシグネチャの生成
	hr = m_dev->CreateRootSignature(
		0,									// ノードマスク。0で良い	
		signatureBlob->GetBufferPointer(),	// バイナリデータ
		signatureBlob->GetBufferSize(),		// バイナリデータのサイズ
		IID_PPV_ARGS(&m_rootSignature));	// ルートシグネチャ

	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateRootSignature Failed.", "Error", MB_OK);
		signatureBlob->Release();
		return false;
	}
	signatureBlob->Release();	// バイナリデータの解放
	return true;
}

bool Renderer::CreatePipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_rootSignature;	// ルートシグネチャ
	// シェーダーの設定
	psoDesc.VS.pShaderBytecode = m_vsBlob->GetBufferPointer();	// VSのバイトコード
	psoDesc.VS.BytecodeLength = m_vsBlob->GetBufferSize();	    // VSのバイトコードのサイズ
	psoDesc.PS.pShaderBytecode = m_psBlob->GetBufferPointer();	// PSのバイトコード
	psoDesc.PS.BytecodeLength = m_psBlob->GetBufferSize();	    // PSのバイトコードのサイズ
	// サンプルマスクとラスタライザステートの設定
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	            // サンプルマスク
	psoDesc.RasterizerState.MultisampleEnable = false;	        // アンチエイリアスは使わないのでfalse
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// カリングモード
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// 中身を塗りつぶす
	psoDesc.RasterizerState.DepthClipEnable = true;	            // 深度クリッピングを有効にする
	// ブレンドステートの設定
	psoDesc.BlendState.AlphaToCoverageEnable = false;	        // アルファツーカバレッジは使わない
	psoDesc.BlendState.IndependentBlendEnable = false;	        // 独立ブレンドは使わない
	// レンダーターゲットの設定
	D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = false;				                    // ブレンドしない
	rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// すべてのカラーチャンネルを書き込む
	psoDesc.BlendState.RenderTarget[0] = rtBlendDesc;	                // レンダーターゲットの設定
	psoDesc.NumRenderTargets = 1;	                                    // レンダーターゲットの数
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;					// レンダーターゲットのフォーマット
	// インプットレイアウトの設定
	psoDesc.InputLayout.pInputElementDescs = m_inputLayout;		// インプットレイアウト
	psoDesc.InputLayout.NumElements = _countof(m_inputLayout);	// インプットレイアウトの数
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// カットなし
	// プリミティブトポロジの設定
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// 三角形リスト
	// アンチエイリアシングのためのサンプル係数の設定
	psoDesc.SampleDesc.Count = 1;	// サンプリングは1ピクセルにつき1回
	psoDesc.SampleDesc.Quality = 0;	// クオリティは最低

	// パイプラインステートの生成
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
	// ビューポートの設定
	m_viewport.Width = static_cast<float>(WINDOW_WIDTH);	    // ウィンドウの幅
	m_viewport.Height = static_cast<float>(WINDOW_HEIGHT);	    // ウィンドウの高さ
	m_viewport.TopLeftX = 0;	                                // 出力先の左上のX座標
	m_viewport.TopLeftY = 0;	                                // 出力先の左上のY座標
	m_viewport.MaxDepth = 1.0f;	                                // 深度値の最大値
	m_viewport.MinDepth = 0.0f;	                                // 深度値の最小値

	// シザリング矩形の設定
	m_scissorRect.top = 0;	                                    // 切り抜き矩形の上端
	m_scissorRect.left = 0;	                                    // 切り抜き矩形の左端
	m_scissorRect.right = m_scissorRect.left + WINDOW_WIDTH;	// 切り抜き矩形の右端
	m_scissorRect.bottom = m_scissorRect.top + WINDOW_HEIGHT;	// 切り抜き矩形の下端

	return true;
}

bool Renderer::CreateTestTexture()
{
	// テクスチャの設定
	m_texData.resize(256 * 256);	// テクスチャデータのサイズ

	for (auto& tex : m_texData)
	{
		tex.r = rand() % 256;	// 赤成分
		tex.g = rand() % 256;	// 緑成分
		tex.b = rand() % 256;	// 青成分
		tex.a = 255;	        // アルファ成分は1.0とする
	}

	// WriteToSubresourceで転送するためのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	// 特殊な設定なのでDEFAULTでもUPLOADでもない
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ライトバック
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// 転送はL0、つまりCPu側から直接行う
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// 単一アダプタのため0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// フォーマット
	resDesc.Width = 256;									// 幅
	resDesc.Height = 256;									// 高さ
	resDesc.DepthOrArraySize = 1;							// 2Dで配列もないので1
	resDesc.SampleDesc.Count = 1;							// 通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Quality = 0;							// クオリティは最低
	resDesc.MipLevels = 1;									// ミップマップは使わない
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	        // レイアウトは決定しない
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;				// 特にフラグはなし

	// リソースの生成
	m_texBuff = nullptr;

	auto hr = m_dev->CreateCommittedResource(
		&heapProp,									// ヒープの設定
		D3D12_HEAP_FLAG_NONE,						// 特にフラグはなし
		&resDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// テクスチャ用指定
		nullptr,									// クリア値
		IID_PPV_ARGS(&m_texBuff)						// テクスチャバッファ
	);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// データ転送
	hr = m_texBuff->WriteToSubresource(
		0,
		nullptr,				// 全領域へのコピー
		m_texData.data(),		// 元データアドレス
		sizeof(TexRGBA) * 256,	// 1行分のサイズ
		sizeof(TexRGBA) * m_texData.size()	// 全データサイズ
	);
	// エラーチェック
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
	// ディスクリプタヒープの設定
	m_pBasicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

	// シェーダーから見えるように
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// マスクは0
	descHeapDesc.NodeMask = 0;
	// SRVとCBVの2つ
	descHeapDesc.NumDescriptors = 2;
	// シェーダーリソースビュー用
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	// 生成
	auto hr = m_dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_pBasicDescHeap));
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateDescriptorHeap Failed.", "Error", MB_OK);
		return false;
	}

	// シェーダーリソースビューの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(0.0f～1.0fに正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// マッピング
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;						// ミップマップは使わない

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = m_pBasicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// シェーダーリソースビューの生成
	m_dev->CreateShaderResourceView(m_texBuff, &srvDesc, basicHeapHandle);

	// 次の場所に移動
	basicHeapHandle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 定数バッファビューの設定
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_constBuff->GetGPUVirtualAddress();	// バッファの仮想アドレス
	cbvDesc.SizeInBytes = m_constBuff->GetDesc().Width;	    // バッファのサイズ

	// 定数バッファビューの生成
	m_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return true;
}

bool Renderer::CreateTexture()
{
	TexMetadata meta = {};
	ScratchImage img = {};

	auto hr = LoadFromWICFile(L"Assets/Texture/field.jpg", WIC_FLAGS_NONE, &meta, img);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "LoadFromWICFile Failed.", "Error", MB_OK);
		return false;
	}

	auto imgData = img.GetImage(0, 0, 0);	// 生データ抽出

	// WriteToSubresourceで転送するためのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	// 特殊な設定なのでDEFAULTでもUPLOADでもない
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ライトバック
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// 転送はL0、つまりCPu側から直接行う
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// 単一アダプタのため0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// フォーマット
	resDesc.Width = meta.width;								// 幅
	resDesc.Height = meta.height;							// 高さ
	resDesc.DepthOrArraySize = meta.arraySize;				// 2Dで配列もないので1
	resDesc.SampleDesc.Count = 1;							// 通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Quality = 0;							// クオリティは最低
	resDesc.MipLevels = meta.mipLevels;						// ミップレベル
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(meta.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	        // レイアウトは決定しない
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;				// 特にフラグはなし

	// リソースの生成
	m_texBuff = nullptr;

	hr = m_dev->CreateCommittedResource(
		&heapProp,									// ヒープの設定
		D3D12_HEAP_FLAG_NONE,						// 特にフラグはなし
		&resDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// テクスチャ用指定
		nullptr,									// クリア値
		IID_PPV_ARGS(&m_texBuff)						// テクスチャバッファ
	);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// データ転送
	hr = m_texBuff->WriteToSubresource(
		0,
		nullptr,				// 全領域へのコピー
		imgData->pixels,		// 元データアドレス
		imgData->rowPitch,		// 1行分のサイズ
		imgData->slicePitch		// 全データサイズ
	);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "WriteToSubresource Failed.", "Error", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::CreateConstantBuffer()
{
	// ワールド行列の設定
	XMMATRIX matrix = XMMatrixIdentity();
	XMMATRIX world = XMMatrixRotationY(XMConvertToRadians(45.0f));
	XMFLOAT3 eye(0, 10, -15);
	XMFLOAT3 target(0, 10, 0);
	XMFLOAT3 up(0, 1, 0);
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 1.0f, 100.0f);

	// バッファの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	                    // アップロードヒープ
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの設定

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元
	resDesc.Width = (sizeof(matrix) + 0xff) & ~0xff;	    // 頂点が入るだけのサイズ
	resDesc.Height = 1;	                                    // 高さ
	resDesc.DepthOrArraySize = 1;	                        // デプス
	resDesc.MipLevels = 1;	                                // ミップマップ
	resDesc.Format = DXGI_FORMAT_UNKNOWN;	                // フォーマット
	resDesc.SampleDesc.Count = 1;	                        // サンプル数
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	            // フラグ
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	    // テクスチャのレイアウト


	auto hr = m_dev->CreateCommittedResource(
		&heapProp,		// アップロードヒープ
		D3D12_HEAP_FLAG_NONE,								// 特にフラグはなし
		&resDesc,			// バッファの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,					// リソースの状態
		nullptr,											// クリア値
		IID_PPV_ARGS(&m_constBuff)							// バッファ
	);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "CreateCommittedResource Failed.", "Error", MB_OK);
		return false;
	}

	// バッファにデータをコピー
	m_pMapMat = nullptr;	// マップ先を示すポインター
	hr = m_constBuff->Map(0, nullptr, (void**)&m_pMapMat);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Map Failed.", "Error", MB_OK);
		return false;
	}

	// 行列の合成
	matrix = world * view * proj;
	*m_pMapMat = matrix;	// 行列の内容をコピー

	return true;
}

bool Renderer::LoadModel()
{

	// モデルの読み込み
	char signature[3] = {};	// シグネチャ
	FILE* fp = fopen("Assets/Model/初音ミク.pmd", "rb");

	fread(signature, sizeof(signature), 1, fp);
	fread(&m_pmdHeader, sizeof(m_pmdHeader), 1, fp);


	fread(&vertNum, sizeof(vertNum), 1, fp);

	vertices.resize(vertNum * pmdvertex_size);	// バッファーの確保
	fread(vertices.data(), vertices.size(), 1, fp);	// 読み込み


	fclose(fp);

	return true;
}

