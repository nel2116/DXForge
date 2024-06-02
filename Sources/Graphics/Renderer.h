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
#include <array>
#include <queue>
#include <System/window.h>

using namespace std;

// ====== クラス定義 ======
class RTVHeap;
class CBVSRVUAVHeap;
class CBufferAllocater;
class DSVHeap;
class DepthStencil;
class ImGuiHeap;
class Renderer
{
private:	// 定数定義
	// GPUのティア
	enum class E_GPU_TIER
	{
		NVIDIA,
		Amd,
		Intel,
		Arm,
		Qualcomm,
		Kind,
	};

private:	// 型定義
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// パブリック関数

	/// <summary>
	/// レンダラーの初期化
	/// </summary>
	/// <param name="window">ウィンドウクラスの参照</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(Window* window);

	/// <summary>
	/// レンダラーの終了処理
	/// </summary>
	/// <returns></returns>
	void Uninit();

	/// <summary>
	/// 3Dレンダリングの開始
	/// </summary>
	/// <returns></returns>
	void Begin3DDraw();

	/// <summary>
	/// 2Dレンダリングの開始
	/// </summary>
	/// <returns></returns>
	void Begin2DDraw();

	/// <summary>
	/// ImGuiの描画開始
	/// </summary>
	/// <returns></returns>
	void BeginImGuiDraw();

	/// <summary>
	/// ImGuiの描画終了
	/// </summary>
	/// <returns></returns>
	void EndImGuiDraw();

	/// <summary>
	/// レンダリングの終了
	/// </summary>
	/// <returns></returns>
	void EndDraw();

	/// <summary>
	/// コマンドキューの同期待ち
	/// </summary>
	void WaitForCmdQueue();

public:		// アクセサ関数
	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイスの参照</returns>
	ID3D12Device8* GetDev()const { return m_pDevice.Get(); }

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns>コマンドリストの参照</returns>
	ID3D12GraphicsCommandList6* GetCmdList()const { return m_pCmdList.Get(); }

	/// <summary>
	/// CBVSRVUAVヒープの取得
	/// </summary>
	/// <returns>CBVSRVUAVヒープのポインタ</returns>
	CBVSRVUAVHeap* GetCBVSRVUAVHeap()const { return m_upCBVSRVUAVHeap.get(); }

	/// <summary>
	/// CBufferアロケータの取得
	/// </summary>
	/// <returns>CBufferアロケータのポインタ</returns>
	CBufferAllocater* GetCBufferAllocater()const { return m_upCBufferAllocater.get(); }

	/// <summary>
	/// DSVヒープの取得
	/// </summary>
	/// <returns>DSVヒープのポインタ</returns>
	DSVHeap* GetDSVHeap()const { return m_upDSVHeap.get(); }

	/// <summary>
	/// imuGuiヒープの取得
	/// </summary>
	/// <returns>imuGuiヒープのポインタ</returns>
	ImGuiHeap* GetImGuiHeap()const { return m_upImGuiHeap.get(); }

	/// <summary>
	/// クリアカラーの設定
	/// </summary>
	/// <param name="r">赤成分</param>
	/// <param name="g">緑成分</param>
	/// <param name="b">青成分</param>
	/// <param name="a">アルファ成分</param>
	void SetClearColor(float r, float g, float b, float a);

	/// <summary>
	/// リソースとして引数に渡したバッファの扱いを変更する関数
	/// </summary>
	/// <param name="pResource">リソースの参照</param>
	/// <param name="before">変更前のリソースの状態</param>
	/// <param name="after">変更後のリソースの状態</param>
	void TransBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:
	/// <summary>
	/// ファクトリーの作成
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool CreateFactory();

	/// <summary>
	/// デバイスの作成
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool CreateDevice();

	/// <summary>
	/// コマンドリストの作成
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool CreateCommandList();

	/// <summary>
	/// スワップチェインの作成
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool CreateSwapChain();

	/// <summary>
	/// スワップチェインRTVの作成
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool CreateSwapChainRTV();

	/// <summary>
	/// Fenceの作成
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool CreateFence();

	/// <summary>
	/// ImGuiのセッティング
	/// </summary>
	/// <returns>作成に成功したらtrue</returns>
	bool ImGuiSetting();

private:	// メンバ変数
	// デバイス
	D3D_FEATURE_LEVEL m_featureLevel = {};
	ComPtr<ID3D12Device8> m_pDevice = nullptr;
	ComPtr<IDXGIFactory6> m_pFactory = nullptr;
	// コマンドリスト
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList6> m_pCmdList = nullptr;
	ComPtr<ID3D12CommandQueue> m_pCmdQueue = nullptr;
	// スワップチェイン
	ComPtr<IDXGISwapChain4> m_pSwapChain = nullptr;
	array<ComPtr<ID3D12Resource>, 2> m_pBackBuffers;
	unique_ptr<RTVHeap> m_pRTVHeap = nullptr;
	// CBVSRVUAVヒープ
	unique_ptr<CBVSRVUAVHeap> m_upCBVSRVUAVHeap = nullptr;
	// CBufferアロケータ
	unique_ptr<CBufferAllocater> m_upCBufferAllocater = nullptr;
	// DSVヒープ
	unique_ptr<DSVHeap> m_upDSVHeap = nullptr;
	// デプスステンシル
	unique_ptr<DepthStencil> m_upDepthStencil = nullptr;
	// imuGuiヒープ
	unique_ptr<ImGuiHeap> m_upImGuiHeap = nullptr;

	// Fance
	ComPtr<ID3D12Fence> m_pFence = nullptr;
	UINT64 m_fanceVal = 0;

	// クリアカラー
	float m_clearColor[4] = {};
	// ウィンドウクラス
	Window* m_pWindow = nullptr;
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
