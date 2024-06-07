// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <wrl/client.h>
#include <System/window.h>

using namespace std;

// ====== 型定義 ======
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

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
	ID3D12CommandList* GetCmdList() { return m_pCmdList.Get(); }

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
	// RTVの生成
	bool CreateRTV();
	// フェンスの生成
	bool CreateFence();

private:	// メンバ変数
	// フレームバッファ数
	static const int FRAME_BUFFER_COUNT = 2;

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

	// レンダーターゲットビュー関連
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;							// RTVヒープ
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle[FRAME_BUFFER_COUNT] = {};	// RTVハンドル
	ComPtr<ID3D12Resource> m_pColorBuffer[FRAME_BUFFER_COUNT];			// カラーバッファ

	// ビューポート関連
	D3D12_VIEWPORT m_viewport;	// ビューポート
	D3D12_RECT m_scissorRect;	// シザー矩形

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