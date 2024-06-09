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
class Polygon;
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
	MyPolygon m_Polygon;

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
	Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};