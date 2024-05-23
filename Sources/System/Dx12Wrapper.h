// [Dx12Wrapper.h]
// 作成日 : 2024/05/21 17:00
// 作成者 : 田中ミノル
// 概要   : DirectX 12ラッパークラスの定義
#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <map>
#include <unordered_map>
#include <DirectXTex.h>
#include <wrl.h>
#include <string.h>
#include <functional>

// 
class Dx12Wrapper
{
	// using宣言
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:	// 構造体定義
	struct SceneData
	{
		DirectX::XMMATRIX view;	// ビュー行列
		DirectX::XMMATRIX proj;	// プロジェクション行列
		DirectX::XMFLOAT3 eye;	// 視点座標
	};

public:		// メンバ関数
	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	void Update();
	void BeginDraw();
	void EndDraw();

	/// <summary>
	/// テクスチャパスから必要なテクスチャバッファへのポインタを返す
	/// @param texPath : テクスチャのパス
	/// </summary>
	ComPtr<ID3D12Resource> GetTextureByPath(const char* texPath);

	/// <summary>
	/// デバイス
	/// </summary>
	ComPtr<ID3D12Device> GetDevice();

	/// <summary>
	/// コマンドリスト
	/// </summary>
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();

	/// <summary>
	/// スワップチェイン
	/// <summary>
	ComPtr<IDXGISwapChain4> GetSwapChain();

	void SetScene();


private:	// プライベート関数
	// 最終的なレンダーターゲットの生成
	HRESULT CreateFinalRenderTargets();
	// デプスステンシルビューの生成
	HRESULT CreateDepthStencilView();

	// スワップチェインの生成
	HRESULT CreateSwapChain(const HWND& hwnd);

	// DXGI関連の初期化
	HRESULT InitDXGIDevice();

	// コマンド関連の初期化
	HRESULT InitCommand();

	// ビュープロジェクション用ビューの生成
	HRESULT CreateSceneView();

	// テクスチャローダテーブルの作成
	void CreateTextureLoaderTable();

	// テクスチャ名からテクスチャバッファ作成、中身をコピー
	ID3D12Resource* CreateTextureFromFile(const char* texPath);

private:	// メンバ変数
	SIZE m_winSize;	// ウィンドウサイズ

	// DXGI関連
	ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;	// DXGIインターフェイス
	ComPtr<IDXGISwapChain4> m_swapChain = nullptr;	// スワップチェイン

	// DirectX12関連
	ComPtr<ID3D12Device> m_device = nullptr;					// デバイス
	ComPtr<ID3D12CommandAllocator> m_cmdAllocator = nullptr;	// コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_cmdList = nullptr;		// コマンドリスト
	ComPtr<ID3D12CommandQueue> m_cmndQueue = nullptr;			// コマンドキュー

	// 表示に関わるバッファ関連
	ComPtr<ID3D12Resource> m_depthBuffer = nullptr;		// 深度バッファ
	vector<ID3D12Resource*> m_backBuffers;				// バックバッファ
	ComPtr<ID3D12DescriptorHeap> m_rtvHeaps = nullptr;	// レンダーターゲットビューヒープ
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;	// 深度ステンシルビューヒープ
	unique_ptr<D3D12_VIEWPORT> m_viewport;				// ビューポート
	unique_ptr<D3D12_RECT> m_scissorRect;				// シザー矩形

	// シーンを構成するバッファ関連
	ComPtr<ID3D12Resource> m_sceneConstBuff = nullptr;	// シーン定数バッファ
	SceneData* m_mappedSceneData;						// マップされたシーンデータ
	ComPtr<ID3D12DescriptorHeap> m_sceneDescHeap = nullptr;	// シーンデスクリプタヒープ


	// フェンス
	ComPtr<ID3D12Fence> m_fence = nullptr;	// フェンス
	UINT64 m_fenceVal = 0;					// フェンスの値

	// ロード用テーブル
	using LoadLambda_t = function<HRESULT(const wstring& path, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	map<string, LoadLambda_t> m_loadLambdaTable;
	// テクスチャテーブル
	unordered_map<string, ComPtr<ID3D12Resource>> m_textureTable;
};