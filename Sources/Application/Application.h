// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Application.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 15:00
// 概要   : アプリケーションクラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/window.h>

// ====== 定数定義 ======
const float FPS = 120.0f;				// フレームレート
const float FRAME_TIME = 1000.0f / FPS;	// フレーム時間

// ====== クラス定義 ======
class Application
{
public:		// パブリック関数
	/// <summary>
	/// アプリケーションの初期化
	/// </summary>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init();

	/// <summary>
	/// アプリケーションの実行
	/// </summary>
	void Run();

	/// <summary>
	/// アプリケーションの終了処理
	/// </summary>
	void Uninit();

private:	// プライベート関数	
	/// <summary>
	///  .dllのディレクトリのセットとロードを行う
	/// </summary>
	void SetDirectoryAndLoadDll();

	void DrawScene();

	void DrawMesh();


private:	// メンバ変数
	Window m_window;
	DWORD m_dwExecLastTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwFrameCount;
	DWORD m_dwLsatFPSTime;
	float m_fFPS;

	// テスト用
	RootSignature m_SceneRootSig;				// ルートシグニチャ
	ColorTarget m_SceneColorTarget;					// シーンカラーターゲット
	DepthTarget m_SceneDepthTarget;					// シーンデプスターゲット
	std::vector<Mesh*> m_pMesh;					// メッシュ
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];          //!< ライトバッファです.
	ConstantBuffer m_CameraCB[FRAME_BUFFER_COUNT];         //!< カメラバッファです.
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];      //!< 変換用バッファです.
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];           //!< メッシュ用バッファです.
	Material m_Material;						// マテリアル
	ComPtr<ID3D12PipelineState> m_pScenePSO;	// パイプラインステート
	float m_RotateAngle;						// 回転角

public:		// シングルトン関連
	/// <summary>
	/// Applicationクラスのインスタンスを取得
	/// </summary>
	/// <returns></returns>
	static Application& Instance()
	{
		static Application instance;
		return instance;
	}
private:	// シングルトン関連
	Application()
		: m_dwExecLastTime(0)
		, m_dwCurrentTime(0)
		, m_dwFrameCount(0)
		, m_dwLsatFPSTime(0)
		, m_fFPS(0.0f)
		, m_RotateAngle(0.0f)
		, m_Material()
		, m_pMesh()
		, m_window()
		, m_SceneRootSig()
		, m_SceneColorTarget()
		, m_SceneDepthTarget()
		, m_LightCB()
		, m_CameraCB()
	{
	}
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};