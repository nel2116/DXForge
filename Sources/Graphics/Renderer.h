// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/window.h>
#include <System/ComPtr.h>
#include <Graphics/ColorTarget.h>
#include <Graphics/DepthTarget.h>

using namespace std;

#define FRAME_BUFFER_COUNT 2	// フレームバッファの数

// 列挙体
enum DescriptorPoolType
{
	POOL_TYPE_RES = 0,     // CBV / SRV / UAV
	POOL_TYPE_SMP = 1,     // Sampler
	POOL_TYPE_RTV = 2,     // RTV
	POOL_TYPE_DSV = 3,     // DSV
	POOL_COUNT = 4,
};

// ====== クラス定義 ======
class Renderer
{
public:		// パブリック関数
	/// <summary>
	/// レンダラーの初期化
	/// </summary>
	/// <param name="window">ウィンドウクラスのポインタ</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(Window* window);

	/// <summary>
	/// レンダラーの終了処理
	/// </summary>
	/// <returns></returns>
	void Uninit();

	/// <summary>
	/// レンダラーの描画開始処理
	/// </summary>
	/// <returns></returns>
	void Begin();

	/// <summary>
	///	レンダラーの描画終了処理
	/// </summary>
	/// <returns></returns>
	void End();

public:		// アクセサ関数
	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイスの参照</returns>
	ID3D12Device* GetDevice() { return m_pDevice.Get(); }

	/// <summary>
	/// コマンドリストの取得
	///	</summary>
	/// <returns>コマンドリストの参照</returns>
	ID3D12GraphicsCommandList* GetCmdList() { return m_pCmdList.Get(); }

	/// <summary>
	/// コマンドキューの取得
	/// </summary>
	/// <returns>コマンドキューの参照</returns>
	ID3D12CommandQueue* GetCmdQueue() { return m_pCmdQueue.Get(); }

	/// <summary>
	/// スワップチェインの取得
	/// </summary>
	/// <returns>スワップチェインの参照</returns>
	IDXGISwapChain3* GetSwapChain() { return m_pSwapChain.Get(); }

	/// <summary>
	/// フレームバッファのインデックスを取得
	/// </summary>
	/// <returns>フレームバッファのインデックス</returns>
	uint32_t GetFrameIndex() { return m_FrameIndex; }

	/// <summary>
	/// カラーターゲットの取得
	/// </summary>
	/// <param name="index">カラーターゲットのインデックス</param>
	/// <returns>カラーターゲット</returns>
	ColorTarget& GetColorTarget(uint32_t index) { return m_ColorTarget[index]; }

	/// <summary>
	/// ウィンドウの横幅を取得
	/// </summary>
	/// <returns>ウィンドウの横幅</returns>
	int GetWidth() { return m_pWindow->GetWidth(); }

	/// <summary>
	/// ウィンドウの縦幅を取得
	/// </summary>
	/// <returns>ウィンドウの縦幅</returns>
	int GetHeight() { return m_pWindow->GetHeight(); }

	/// <summary>
	/// ビューポートの取得
	/// </summary>
	/// <returns>ビューポート</returns>
	D3D12_VIEWPORT GetViewport() { return m_viewport; }

	/// <summary>
	/// シザー矩形の取得
	/// </summary>
	/// <returns>シザー矩形</returns>
	D3D12_RECT GetScissor() { return m_scissor; }

	/// <summary>
	/// ディスクリプタプールの取得
	/// </summary>
	/// <param name="type">ディスクリプタプールの種類</param>
	/// <returns>ディスクリプタプールの参照</returns>
	DescriptorPool* GetDescriptorPool(DescriptorPoolType type) { return m_pPool[type]; }

	/// <summary>
	/// デプスターゲットの取得
	/// </summary>
	/// <returns>デプスターゲット</returns>
	DepthTarget& GetDepthTarget() { return m_DepthTarget; }

private:	// プライベート関数

	// 画面フリップ
	void Present(uint32_t interval);
	// GPUの処理完了を待つ
	void WaitGpu();

	// デバイスの生成
	bool CreateDevice();
	// コマンドキューの生成
	bool CreateCommandQueue();
	// スワップチェインの生成
	bool CreateSwapChain();
	// コマンドアロケータの生成
	bool CreateCommandAllocator();
	// コマンドリストの生成
	bool CreateCommandList();
	// フェンスの生成
	bool CreateFence();
	// レンダーターゲットビューの生成
	bool CreateRenderTargetView();
	// 深度ステンシルビューの生成
	bool CreateDepthStencilView();
	// ディスクリプタプールの生成
	bool CreateDescriptorPool();

private:	// メンバ変数

	// ウィンドウ関連
	Window* m_pWindow = nullptr;	// ウィンドウクラスのポインタ

	// デバイス関連
	ComPtr<ID3D12Device> m_pDevice;										// デバイス
	ComPtr<ID3D12CommandQueue> m_pCmdQueue;								// コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain;								// スワップチェイン
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FRAME_BUFFER_COUNT];	// コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;						// コマンドリスト

	// フェンス関連
	ComPtr<ID3D12Fence> m_pFence;										// フェンス
	HANDLE m_fenceEvent = nullptr;										// フェンスイベント
	uint64_t m_FenceCounter[FRAME_BUFFER_COUNT] = {};					// フェンスカウンタ
	uint32_t m_FrameIndex = 0;											// フレーム番号

	// バックバッファ関連
	ColorTarget m_ColorTarget[FRAME_BUFFER_COUNT];	// カラーターゲット
	DepthTarget m_DepthTarget;						// 深度ターゲット

	DescriptorPool* m_pPool[POOL_COUNT];	// ディスクリプタプール

	// ビューポート関連
	D3D12_VIEWPORT m_viewport;	// ビューポート
	D3D12_RECT m_scissor;	// シザー矩形

public:
	// シングルトン関連
	/// <summary>
	/// Rendererクラスのインスタンスを取得
	/// </summary>
	/// <returns>Rendererクラスのインスタンス参照</returns>
	static Renderer& Instance()
	{
		static Renderer instance;
		return instance;
	}
private:
	Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
};

#define RENDERER Renderer::Instance()	// レンダラーのインスタンスを取得
#define WIDTH RENDERER.GetWidth()		// ウィンドウの幅
#define HEIGHT RENDERER.GetHeight()		// ウィンドウの高さ