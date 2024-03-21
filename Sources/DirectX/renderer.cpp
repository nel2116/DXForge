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
	// 解放処理

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

	// 正常に初期化できた
	return true;
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
	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };	// 黄色
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);


	// ----- 描画命令を挟む -----
	m_cmdList->RSSetViewports(1, &m_viewport);	                            // ビューポートの設定
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);	                    // シザリング矩形の設定
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);	                // ルートシグネチャの設定
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブトポロジの設定
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBuffView);	                // 頂点バッファの設定
	m_cmdList->IASetIndexBuffer(&m_indexBuffView);	                        // インデックスバッファの設定
	m_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);	                        // 描画命令

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
	// フェンスの生成
	auto hr = m_dev->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

	return true;
}

bool Renderer::CreateVertexBuffer()
{
	// 頂点情報の設定
	XMFLOAT3 vertices[4] =
	{
		{-0.4f, -0.7f, 0.0f},	// 左下
		{-0.4f, 0.7f, 0.0f},	// 左上
		{0.4f, -0.7f, 0.0f},	// 右下
		{0.4f, 0.7f, 0.0f},		// 右上
	};

	// バッファの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	                    // アップロードヒープ
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUページプロパティ
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールの設定

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元
	resDesc.Width = sizeof(vertices);	                    // 頂点が入るだけのサイズ
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

	// バッファにデータをコピー
	void* p = nullptr;
	m_vertexBuff->Map(0, nullptr, &p);
	memcpy(p, vertices, sizeof(vertices));
	m_vertexBuff->Unmap(0, nullptr);

	// バッファビューの設定
	m_vertexBuffView.BufferLocation = m_vertexBuff->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_vertexBuffView.SizeInBytes = sizeof(vertices);	                    // バッファのサイズ
	m_vertexBuffView.StrideInBytes = sizeof(XMFLOAT3);	                    // ストライド

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
	m_indexBuff->Map(0, nullptr, &p);
	memcpy(p, indices, sizeof(indices));
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

	return true;
}

bool Renderer::CreateRootSignature()
{
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 入力アセンブラ入力レイアウトを許可

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
