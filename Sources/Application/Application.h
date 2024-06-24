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


private:	// メンバ変数
	Window m_window;
	DWORD m_dwExecLastTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwFrameCount;
	DWORD m_dwLsatFPSTime;
	float m_fFPS;

	// テスト用
	std::vector<Mesh*> m_pMesh;					// メッシュ
	std::vector<ConstantBuffer*> m_Transform;	// 変換行列
	ConstantBuffer* m_pLight;					// ライト
	Material m_Material;						// マテリアル
	ComPtr<ID3D12PipelineState> m_pPSO;			// パイプラインステート
	ComPtr<ID3D12RootSignature> m_pRootSig;		// ルートシグニチャ
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
		, m_pLight(nullptr)
		, m_pPSO(nullptr)
		, m_pRootSig(nullptr)
		, m_Material()
		, m_pMesh()
		, m_Transform()
		, m_window()
	{
	}
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};