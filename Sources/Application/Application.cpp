//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Application.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : アプリケーションクラスの実装
// 更新履歴 : 2024/05/25 作成
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Application.h"
#include <Manager/ActorManager.h>

// ====== メンバ関数 ======
bool Application::Init()
{
	SetDirectoryAndLoadDll();	// .dllのディレクトリのセットとロードを行う

	// COMの初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		MessageBox(nullptr, "COMの初期化に失敗しました。", "エラー", MB_OK);
		assert(0 && "COMの初期化に失敗しました。");
		return false;
	}

	if (!m_window.Create(WINDOW_WIDTH, WINDOW_HEIGHT, CLASS_NAME, WINDOW_NAME))
	{
		assert(0 && "ウィンドウの作成に失敗しました。");
		return false;
	}

	if (!RENDERER.Init(&m_window))
	{
		assert(0 && "Rendererの初期化に失敗しました。");
		return false;
	}

	// ====== Managerの初期化 ======
	ACTOR_MANAGER.Init();

	if (!m_Polygon.Init())
	{
		assert(0 && "ポリゴンの初期化に失敗しました。");
		return false;
	}

	// タイマーの初期化
	timeBeginPeriod(1);
	m_dwExecLastTime = timeGetTime();
	m_dwCurrentTime = m_dwExecLastTime;
	m_dwFrameCount = 0;
	m_dwLsatFPSTime = m_dwExecLastTime;
	m_fFPS = 0.0f;
	return true;
}

void Application::Run()
{
	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }

		m_dwCurrentTime = timeGetTime();
		if ((m_dwCurrentTime - m_dwExecLastTime) >= FRAME_TIME)
		{
			m_dwExecLastTime = m_dwCurrentTime;
			// ====== ここにゲームの処理を書く ======
			// ------ 更新処理 ------
			ACTOR_MANAGER.Update();

			// ------ 描画処理 ------
			RENDERER.Begin();

			// --- 以下ゲームの描画処理
			ACTOR_MANAGER.Draw();
			m_Polygon.Draw();

			// --- ここまでゲームの描画処理

			// ----- 描画終了 -----
			RENDERER.End();
			// ========================================
			m_dwFrameCount++;
		}
	}
}

void Application::Uninit()
{
	// ====== 終了処理 ======
	m_Polygon.Uninit();	// ポリゴンの終了処理

	// ----- Managerの終了処理 -----
	ACTOR_MANAGER.Uninit();	// ActorManagerの終了処理

	// ----- システムの終了処理 -----
	RENDERER.Uninit();	// Rendererの終了処理
	timeEndPeriod(1);	// タイマーの終了処理
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// ウィンドウクラスの登録解除
	CoUninitialize();	// COMの終了処理
}

void Application::SetDirectoryAndLoadDll()
{
#ifdef _DEBUG
	SetDllDirectory("Library/assimp/build/lib/Debug");
	LoadLibraryExA("assimp-vc143-mtd.dll", NULL, NULL);
#else
	SetDllDirectory("Library/assimp/build/lib/Release");
	LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif // _DEBUG

}
