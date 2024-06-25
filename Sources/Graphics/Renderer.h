// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの定義
// 更新履歴
// 2024/05/25 作成
// 2024/06/24 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Manager/Manager.h>
#include <System/window.h>
#include <System/ComPtr.h>
#include "Buffer/ConstantBuffer/ConstantBuffer.h"
#include "Buffer/VertexBuffer/VertexBuffer.h"
#include "RootSignature/RootSignature.h"
#include "Target/ColorTarget.h"
#include "Target/DepthTarget.h"
#include "CommandList/CommandList.h"
#include "Fence/Fence.h"

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
class Renderer : public Manager<Renderer>
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
	void End(ColorTarget* sceneTarget);

	/// <summary>
	/// リソースの状態遷移
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="stateBefore"></param>
	/// <param name="stateAfter"></param>
	void TransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	/// <summary>
	/// GPUの完了を待つ
	/// </summary>
	void WaitGpu();

public:		// アクセサ関数

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return m_pDevice.Get(); }

	/// <summary>
	/// コマンドキューの取得
	/// </summary>
	/// <returns>コマンドキュー</returns>
	ID3D12CommandQueue* GetCmdQueue() const { return m_pQueue.Get(); }

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns>コマンドリスト</returns>
	CommandList* GetCmdList() { return &m_CommandList; }

	/// <summary>
	/// スワップチェインの取得
	/// </summary>
	/// <returns>スワップチェイン</returns>
	IDXGISwapChain4* GetSwapChain() const { return m_pSwapChain.Get(); }

	/// <summary>
	/// ディスクリプタプールの取得
	/// </summary>
	/// <param name="type">ディスクリプタプールの種類</param>
	/// <returns>ディスクリプタプール</returns>
	DescriptorPool* GetPool(DescriptorPoolType type) const { return m_pPool[type]; }

	/// <summary>
	/// フレーム番号の取得
	/// </summary>
	/// <returns>フレーム番号</returns>
	uint32_t GetFrameIndex() const { return m_FrameIndex; }

	/// <summary>
	/// カラーターゲットの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>カラーターゲット</returns>
	ColorTarget* GetColorTarget(uint32_t index) { return &m_ColorTarget[index]; }

	/// <summary>
	/// 深度ターゲットの取得
	/// </summary>
	/// <returns>深度ターゲット</returns>
	DepthTarget* GetDepthTarget() { return &m_DepthTarget; }

	/// <summary>
	/// ビューポートの取得
	/// </summary>
	/// <returns>ビューポート</returns>
	const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }

	/// <summary>
	/// シザー矩形の取得
	/// </summary>
	/// <returns>シザー矩形</returns>
	const D3D12_RECT& GetScissor() const { return m_Scissor; }

	/// <summary>
	/// windowの取得
	/// </summary>
	/// <returns>ウィンドウクラスのポインタ</returns>
	Window* GetWindow() const { return m_pWindow; }

	/// <summary>
	/// windowの縦幅の取得
	/// </summary>
	/// <returns>ウィンドウの縦幅</returns>
	float GetHeight() const { return static_cast<float>(m_pWindow->GetHeight()); }

	/// <summary>
	/// windowの横幅の取得
	/// </summary>
	/// <returns>ウィンドウの横幅</returns>
	float GetWidth() const { return static_cast<float>(m_pWindow->GetWidth()); }

private:	// プライベート関数
	void Present(uint32_t interval);
	void CheckSupportHDR();
	bool  IsSupportHDR() const;
	float GetMaxLuminance() const;
	float GetBaseLuminance() const;
	void DrawTonemap(ColorTarget* sceneTarget);

private:	// メンバ変数
	// ウィンドウ関連
	Window* m_pWindow = nullptr;					// ウィンドウクラスのポインタ
	ComPtr<IDXGIFactory4> m_pFactory;				// DXGIファクトリー
	ComPtr<ID3D12Device> m_pDevice;					// デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue;			// コマンドキュー
	ComPtr<IDXGISwapChain4> m_pSwapChain;			// スワップチェイン
	ColorTarget m_ColorTarget[FRAME_BUFFER_COUNT];	// カラーターゲット
	DepthTarget m_DepthTarget;						// 深度ターゲット
	DescriptorPool* m_pPool[POOL_COUNT];			// ディスクリプタプール
	CommandList m_CommandList;						// コマンドリスト
	Fence m_Fence;									// フェンス
	uint32_t m_FrameIndex;							// フレーム番号
	D3D12_VIEWPORT m_Viewport;						// ビューポート
	D3D12_RECT m_Scissor;							// シザー矩形
	DXGI_FORMAT m_BackBufferFormat;					// バックバッファフォーマット
	VertexBuffer m_QuadVB;							// 頂点バッファ

	// トーンマップ関連
	RootSignature m_TonemapRootSig;						// トーンマップ用ルートシグニチャ
	ConstantBuffer m_TonemapCB[FRAME_BUFFER_COUNT];		// 定数バッファ
	ComPtr<ID3D12PipelineState> m_pTonemapPSO;			// パイプラインステート
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// 変換用バッファです.
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// メッシュ用バッファです.
	bool m_IsHDR;										// HDRをサポートしているかどうか
	float m_MaxLuminance;								// 最大輝度
	float m_BaseLuminance;								// 最小輝度
	int m_TonemapType;									// トーンマップタイプ
	int m_ColorSpace;									// 出力色空間

	// ウィンドウ関連
	bool m_IsFullScreen;	// フルスクリーンかどうか
	bool m_IsVSync;			// 垂直同期を行うかどうか

private:	// シングルトン関連
	friend class Manager<Renderer>;
	Renderer(DXGI_FORMAT format)
		: m_BackBufferFormat(format)
		, m_FrameIndex(0)
		, m_IsFullScreen(false)
		, m_IsVSync(true)
		, m_IsHDR(false)
		, m_MaxLuminance(100.0f)
		, m_BaseLuminance(100.0f)
		, m_pWindow(nullptr)
		, m_pFactory(nullptr)
		, m_pDevice(nullptr)
		, m_pQueue(nullptr)
		, m_pSwapChain(nullptr)
		, m_CommandList()
		, m_Fence()
		, m_Viewport()
		, m_Scissor()
		, m_TonemapType(0)
		, m_ColorSpace(0)
		, m_QuadVB()
		, m_TonemapRootSig()
		, m_pTonemapPSO(nullptr)
		, m_TonemapCB()
		, m_TransformCB()
		, m_MeshCB()
		, m_pPool()
		, m_ColorTarget()
		, m_DepthTarget()
	{
		for (auto i = 0; i < POOL_COUNT; ++i)
		{
			m_pPool[i] = nullptr;
		}
	}

	static Renderer* createInstance()
	{
		return NEW Renderer(DXGI_FORMAT_R8G8B8A8_UNORM);
	}
};

// インスタンスの取得
#define RENDERER Renderer::manager()
// ウィンドウの縦幅の取得
#define HEIGHT RENDERER.GetHeight()
// ウィンドウの横幅の取得
#define WIDTH RENDERER.GetWidth()



