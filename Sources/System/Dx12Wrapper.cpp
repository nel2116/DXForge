// [Dx12Wrapper.cpp]
// 作成日 : 2024/05/23 15:00
// 作成者 : 田中ミノル
// 概要   : DirectX 12ラッパークラスの実装
#include "stdafx.h"
#include "Dx12Wrapper.h"
#include <cassert>
#include <DirectX/d3dx12.h>
#include <Application/Application.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

namespace
{
	/// <summary>
	/// モデルのパスとテクスチャのパスから合成パスを得る
	/// @param modelPath : Applicationから見たPMDモデルのパス
	/// @param texPath : PMDモデルから見たテクスチャのパス
	/// @return : Applicationから見たテクスチャのパス
	/// </summary>
	string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath)
	{
		// ファイルのフォルダ区切りは\と/のに種類が使用される可能性があり
		// ともかく末尾の\か/を得られればいいので、双方のrfindを取り、比較する
		// int型二代入しているのは見つからなかった場合はrfindがepos(-1→0xffffffff)を返すため
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	/// <summary>
	/// ファイル名から拡張子を取得する
	/// @param filePath : ファイルパス
	/// @return : ファイルの拡張子
	/// </summary>
	string GetExtension(const string& filePath)
	{
		int idx = filePath.rfind('.');
		return filePath.substr(idx + 1, filePath.length() - idx - 1);
	}

	/// <summary>
	/// ファイル名から拡張子を取得する(ワイド文字版)
	/// @param filePath : ファイルパス
	/// @return : ファイルの拡張子
	/// </summary>
	wstring GetExtension(const wstring& filePath)
	{
		int idx = filePath.rfind(L'.');
		return filePath.substr(idx + 1, filePath.length() - idx - 1);
	}

	/// <summary>
	/// テクスチャのパスをセパレータ文字で分離する
	/// @param texPath : テクスチャのパス
	/// @param separator : 区切り文字
	/// @return : 分離前後の文字列ペア
	/// </summary>
	pair<string, string> SplitFileName(const string& texPath, const char separator = '*')
	{
		int idx = texPath.find(separator);
		pair<string, string> ret;
		ret.first = texPath.substr(0, idx);
		ret.second = texPath.substr(idx + 1, texPath.length() - idx - 1);
		return ret;
	}

	/// <summary>
	/// string(マルチバイト文字)からwstring(ワイド文字)に変換する
	/// @param str : 変換元の文字列
	/// @return : 変換後の文字列
	/// </summary>
	wstring GetWideStringFromString(const string& str)
	{
		// 呼び出し一回目(文字列数を得る)
		int num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

		wstring wstr;	// stringのwchar_t版
		wstr.resize(num1);	// wchar_t版のstringをリサイズ

		// 呼び出し二回目(確保済みのwstrに変換文字列をコピー)
		auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

		assert(num1 == num2);	// 一回目と二回目で文字列数が違うのはおかしい
		return wstr;
	}

	/// <summary>
	/// デバッグレイヤーを有効にする
	/// </summary>
	void EnableDebugLayer()
	{
		ComPtr<ID3D12Debug> debugLayer = nullptr;
		auto res = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
		debugLayer->EnableDebugLayer();
	}
}

Dx12Wrapper::Dx12Wrapper(HWND hwnd)
{
#ifdef _DEBUG
	// デバッグレイヤーを有効にする
	EnableDebugLayer();
#endif // _DEBUG

	auto& app = Application::Instance();
	m_winSize = app.GetWindowSize();

	// DirectX12関連の初期化
	if (FAILED(InitDXGIDevice())) { assert(0); return; }
	if (FAILED(InitCommand())) { assert(0); return; }
	if (FAILED(CreateSwapChain(hwnd))) { assert(0); return; }
	if (FAILED(CreateFinalRenderTargets())) { assert(0); return; }
	if (FAILED(CreateSceneView())) { assert(0); return; }

	// テクスチャローダー関連の初期化
	CreateTextureLoaderTable();

	// 深度バッファの生成
	if (FAILED(CreateDepthStencilView())) { assert(0); return; }

	// フェンスの作成
	if (FAILED(m_device->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()))))
	{
		assert(0);
		return;
	}
}

Dx12Wrapper::~Dx12Wrapper()
{

}

void Dx12Wrapper::Update()
{
}

void Dx12Wrapper::BeginDraw()
{
	// Direct3Dの描画処理開始
	// バックバッファのインデックスを取得
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの指定
	auto rtvH = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 深度を指定
	auto dsvH = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	m_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 画面クリア
	float clearColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };	// 紫色
	m_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// ビューポートとシザー矩形の設定
	m_cmdList->RSSetViewports(1, m_viewport.get());
	m_cmdList->RSSetScissorRects(1, m_scissorRect.get());
}

void Dx12Wrapper::EndDraw()
{
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_cmdList->ResourceBarrier(1, &barrier);

	// コマンドのクローズ
	m_cmdList->Close();

	// コマンドの実行
	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmndQueue->ExecuteCommandLists(1, cmdLists);
	//// 待ち
	m_cmndQueue->Signal(m_fence.Get(), ++m_fenceVal);

	if (m_fence->GetCompletedValue() < m_fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	m_cmdAllocator->Reset(); // キューをリセット
	m_cmdList->Reset(m_cmdAllocator.Get(), nullptr); // コマンドリストをリセット
}

ComPtr<ID3D12Resource> Dx12Wrapper::GetTextureByPath(const char* texPath)
{
	auto it = m_textureTable.find(texPath);
	if (it != m_textureTable.end())
	{
		// テーブル内にあったらロードするのではなくマップ内のリソースを返す
		return m_textureTable[texPath];
	}
	else
	{
		return ComPtr<ID3D12Resource>(CreateTextureFromFile(texPath));
	}
}

ComPtr<ID3D12Device> Dx12Wrapper::GetDevice()
{
	return m_device;
}

ComPtr<ID3D12GraphicsCommandList> Dx12Wrapper::GetCommandList()
{
	return m_cmdList;
}

ComPtr<IDXGISwapChain4> Dx12Wrapper::GetSwapChain()
{
	return m_swapChain;
}

void Dx12Wrapper::SetScene()
{
	// 現在のシーン(ビュープロジェクション)をセット
	ID3D12DescriptorHeap* heaps[] = { m_sceneDescHeap.Get() };
	m_cmdList->SetDescriptorHeaps(1, heaps);
	m_cmdList->SetGraphicsRootDescriptorTable(0, m_sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

HRESULT Dx12Wrapper::CreateFinalRenderTargets()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto hr = m_swapChain->GetDesc1(&desc);
	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "[CreateFinalRenderTargets]スワップチェインの取得に失敗しました", "エラー", MB_OK);
		return hr;
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// レンダーターゲットビュー用
	heapDesc.NodeMask = 0;	// マルチGPUの場合に使用
	heapDesc.NumDescriptors = 2;	// レンダーターゲットビューは2つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// フラグは特になし

	hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_rtvHeaps.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "[CreateFinalRenderTargets]レンダーターゲットビュー用デスクリプタヒープの生成に失敗しました", "エラー", MB_OK);
		SUCCEEDED(hr);
		return hr;
	}

	// レンダーターゲットビューの作成
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = m_swapChain->GetDesc(&swcDesc);
	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "[CreateFinalRenderTargets]スワップチェインの取得に失敗しました", "エラー", MB_OK);
		return hr;
	}
	m_backBuffers.resize(swcDesc.BufferCount);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	// SRGBフォーマットのレンダーターゲットビューを作成
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// SRGBフォーマット
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// テクスチャとして扱う

	for (int i = 0; i < swcDesc.BufferCount; ++i)
	{
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
		assert(SUCCEEDED(hr));
		rtvDesc.Format = m_backBuffers[i]->GetDesc().Format;
		m_device->CreateRenderTargetView(m_backBuffers[i], &rtvDesc, handle);
		handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	m_viewport.reset(new CD3DX12_VIEWPORT(m_backBuffers[0]));
	m_scissorRect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));
	return hr;
}

HRESULT Dx12Wrapper::CreateDepthStencilView()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto hr = m_swapChain->GetDesc1(&desc);

	// 深度バッファの作成
	// 深度バッファの使用
	// auto depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, 
	//	desc.Width, desc.Height, 
	// 1, 0, 1, 0, 
	// D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resdesc.DepthOrArraySize = 1;
	resdesc.Width = desc.Width;
	resdesc.Height = desc.Height;
	resdesc.Format = DXGI_FORMAT_D32_FLOAT;
	resdesc.SampleDesc.Count = 1;
	resdesc.SampleDesc.Quality = 0;
	resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resdesc.MipLevels = 1;
	resdesc.Alignment = 0;


	// デプス用ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	hr = m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// デプス書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(m_depthBuffer.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return hr; }

	// 深度のためのデスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};	// 深度に使うよということがわかれば良い
	dsvHeapDesc.NumDescriptors = 1;	// 深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	// 深度ビュー用
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// フラグは特になし
	hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { return hr; }

	// 深度ビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // デプス値に32bit使用
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // テクスチャとして扱う
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // フラグは特になし
	m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// 問題なく作成できた
	return S_OK;
}

// スワップチェインの生成
HRESULT Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	RECT rc = {};
	::GetWindowRect(hwnd, &rc);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_winSize.cx;
	swapChainDesc.Height = m_winSize.cy;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto hr = m_dxgiFactory->CreateSwapChainForHwnd(m_cmndQueue.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)m_swapChain.ReleaseAndGetAddressOf());

	// エラーチェック
	assert(SUCCEEDED(hr));

	return hr;
}

HRESULT Dx12Wrapper::InitDXGIDevice()
{
	UINT flagsDXGI = 0;
#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	auto hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf()));

	// エラーチェック
	if (FAILED(hr)) { return hr; }

	// DirectX12関連の初期化
	// フィチャーレベル列挙
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	// デバイスの生成
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	// アダプターの列挙
	for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	hr = S_FALSE;
	// Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;
	for (auto l : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()))))
		{
			featureLevel = l;
			hr = S_OK;
			break;
		}
	}

	return hr;
}

HRESULT Dx12Wrapper::InitCommand()
{
	// コマンドキューの生成
	auto hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { assert(0); return hr; }

	// コマンドリストの生成
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_cmdList.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr)) { assert(0); return hr; }


	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// タイムアウトなし
	cmdQueueDesc.NodeMask = 0;	// マルチGPUの場合に使用
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// プライオリティ特に指定なし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// コマンドリストと合わせる
	hr = m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_cmndQueue.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));
	return hr;
}

// ビュープロジェクション用ビューの生成
HRESULT Dx12Wrapper::CreateSceneView()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto hr = m_swapChain->GetDesc1(&desc);
	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "スワップチェインの取得に失敗しました", "エラー", MB_OK);
		return hr;
	}

	// 定数バッファ作成
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);	// アップロード用ヒープ
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);	// シーンデータのサイズ
	hr = m_device->CreateCommittedResource(
		&heapProp,	// アップロード用ヒープ
		D3D12_HEAP_FLAG_NONE,	// 特にフラグはなし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// リソースの状態
		nullptr,	// クリア値
		IID_PPV_ARGS(m_sceneConstBuff.ReleaseAndGetAddressOf()));

	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "シーン定数バッファの生成に失敗しました", "エラー", MB_OK);
		assert(SUCCEEDED(hr));
		return hr;
	}

	// マップ
	m_mappedSceneData = nullptr;	// マップ先を示すポインター
	hr = m_sceneConstBuff->Map(0, nullptr, (void**)&m_mappedSceneData);	// マップ
	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "シーン定数バッファのマップに失敗しました", "エラー", MB_OK);
		assert(SUCCEEDED(hr));
		return hr;
	}

	// ビュー行列、プロジェクション行列の生成
	XMFLOAT3 eye(0.0f, 15.0f, -15.0f);	// 視点
	XMFLOAT3 target(0.0f, 15.0f, 0.0f);	// 注視点
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);		// 上方向
	m_mappedSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));	// ビュー行列
	m_mappedSceneData->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,	// 画角は45°
		static_cast<float>(desc.Width) / static_cast<float>(desc.Height),	// アスペクト比
		0.1f,
		1000.0f);	// プロジェクション行列
	m_mappedSceneData->eye = eye;

	// デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダから見えるように
	descHeapDesc.NodeMask = 0;	// マルチGPUの場合に使用
	descHeapDesc.NumDescriptors = 1;	// デスクリプタは1つ
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// CBV,SRV,UAV用
	hr = m_device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(m_sceneDescHeap.ReleaseAndGetAddressOf()));

	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "シーンデスクリプタヒープの生成に失敗しました", "エラー", MB_OK);
		assert(SUCCEEDED(hr));
		return hr;
	}

	// デスクリプタヒープの先頭アドレスを取得
	auto heapHandle = m_sceneDescHeap->GetCPUDescriptorHandleForHeapStart();
	// デスクリプタサイズを取得
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_sceneConstBuff->GetGPUVirtualAddress();	// バッファの仮想アドレス
	cbvDesc.SizeInBytes = m_sceneConstBuff->GetDesc().Width;	// サイズは16バイトアライメント

	// 定数バッファビューの作成
	m_device->CreateConstantBufferView(&cbvDesc, heapHandle);
	return hr;
}

// テクスチャローダテーブルの作成
void Dx12Wrapper::CreateTextureLoaderTable()
{
	m_loadLambdaTable["sph"] = m_loadLambdaTable["spa"] = m_loadLambdaTable["bmp"] = m_loadLambdaTable["png"] = m_loadLambdaTable["jpg"] = [](const wstring& path, TexMetadata* metadata, ScratchImage& img)->HRESULT
	{
		return LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, metadata, img);
	};
	m_loadLambdaTable["tga"] = [](const wstring& path, TexMetadata* metadata, ScratchImage& img)->HRESULT
	{
		return LoadFromTGAFile(path.c_str(), metadata, img);
	};
	m_loadLambdaTable["dds"] = [](const wstring& path, TexMetadata* metadata, ScratchImage& img)->HRESULT
	{
		return LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, metadata, img);
	};
}

// テクスチャ名からテクスチャバッファ作成、中身をコピー
ID3D12Resource* Dx12Wrapper::CreateTextureFromFile(const char* texPath)
{
	string texPathStr = texPath;
	// テクスチャのロード
	TexMetadata metadata = {};
	ScratchImage ScratchImg = {};
	auto wTexPath = GetWideStringFromString(texPathStr);	// テクスチャのファイルパス
	auto ext = GetExtension(texPathStr);	// ファイルの拡張子を取得
	auto hr = m_loadLambdaTable[ext](wTexPath, &metadata, ScratchImg);
	// エラーチェック
	if (FAILED(hr))
	{
		// メッセージボックスでエラーを表示
		MessageBox(nullptr, "テクスチャの読み込みに失敗しました", "エラー", MB_OK);
		return nullptr;
	}

	auto img = ScratchImg.GetImage(0, 0, 0);	// 生データ抽出

	// WriteToSubresourceで転送する用のヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

	ID3D12Resource* texBuff = nullptr;
	hr = m_device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特にフラグはなし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "テクスチャバッファの生成に失敗しました", "エラー", MB_OK);
		return nullptr;
	}

	hr = texBuff->WriteToSubresource(0, nullptr, img->pixels, img->rowPitch, img->slicePitch);
	// エラーチェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "テクスチャバッファへの書き込みに失敗しました", "エラー", MB_OK);
		return nullptr;
	}

	// 問題がなければ
	return texBuff;
}
