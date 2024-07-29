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
#include <System/Input.h>

using namespace DirectX::SimpleMath;

// ====== メンバ関数 ======
bool Application::Init()
{
	SetDirectoryAndLoadDll();	// .dllのディレクトリのセットとロードを行う

	// COMの初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		ELOG("[App.cpp]Error : Line56 : COMの初期化に失敗しました。");
		return false;
	}

	if (!m_window.Create(WINDOW_WIDTH, WINDOW_HEIGHT, CLASS_NAME, WINDOW_NAME))
	{
		ELOG("[App.cpp]Error : Line62 : ウィンドウの生成に失敗しました。");
		return false;
	}

	if (!RENDERER.Init(&m_window))
	{
		ELOG("[App.cpp]Error : Line68 : Rendererの初期化に失敗しました。");
		return false;
	}

	auto hr = InitInput();
	if (FAILED(hr))
	{
		ELOG("[App.cpp]Error : Line75 : 入力の初期化に失敗しました。%s", hr);
		return false;
	}

	// TimerManagerの初期化
	TIMER_MANAGER.Init();

	// ShaderManagerの初期化
	SHADER_MANAGER.Init();

	// SceneManagerの初期化
	SCENE_MANAGER.Init();

	return true;
}

void Application::Run()
{
	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }

		// タイマーの更新
		TIMER_MANAGER.Update();

		if (TIMER_MANAGER.IsFrameGo())
		{
			// ====== ここにゲームの処理を書く ======
			// ------ 更新処理 ------
			// 入力の更新
			UpdateInput();

			// SceneManagerの更新
			SCENE_MANAGER.Update();
			RENDERER.Update();

			// ------ 描画処理 ------
			RENDERER.Begin();

			// シーンの描画
			SCENE_MANAGER.Draw();

			// --- ここまでゲームの描画処理

			// ----- 描画終了 -----
			RENDERER.End();

			// ========================================
			TIMER_MANAGER.UpdateFrame();
		}
	}
}

void Application::Uninit()
{
	// ====== 終了処理 ======
	RENDERER.WaitGpu();

	// マネージャーの解放
	SCENE_MANAGER.Uninit();
	SHADER_MANAGER.Uninit();
	TIMER_MANAGER.Uninit();

	// ----- システムの終了処理 -----
	UninitInput();	// 入力の終了処理
	RENDERER.Uninit();	// Rendererの終了処理
	timeEndPeriod(1);	// タイマーの終了処理
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// ウィンドウクラスの登録解除
	CoUninitialize();	// COMの終了処理
}

void Application::SetDirectoryAndLoadDll()
{
#ifdef _DEBUG
	// デバッグビルドの場合はデバッグ用のDLLを読み込む
	// AssimpのDLLを読み込む
	// DLLのディレクトリを設定
	SetDllDirectory("Library/assimp/build/lib/Debug");
	// DLLの読み込み
	LoadLibraryExA("assimp-vc143-mtd.dll", NULL, NULL);

	// PhysXのDLLを読み込む
	// DLLのディレクトリを設定
	SetDllDirectory("Library/PhysX/bin/Debug");
	// DLLの読み込み
	LoadLibraryExA("freeglutd.dll", NULL, NULL);
	LoadLibraryExA("PhysX_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCommon_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCooking_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXDevice64.dll", NULL, NULL);
	LoadLibraryExA("PhysXFoundation_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXGpu_64.dll", NULL, NULL);
	LoadLibraryExA("PVDRuntime_64.dll", NULL, NULL);
#else
	// リリースビルドの場合はリリース用のDLLを読み込む
	// AssimpのDLLを読み込む
	// DLLのディレクトリを設定
	SetDllDirectory("Library/assimp/build/lib/Release");
	// DLLの読み込み
	LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);

	// PhysXのDLLを読み込む
	// DLLのディレクトリを設定
	SetDllDirectory("Library/PhysX/bin/Debug");
	// DLLの読み込み
	LoadLibraryExA("freeglutd.dll", NULL, NULL);
	LoadLibraryExA("PhysX_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCommon_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCooking_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXDevice64.dll", NULL, NULL);
	LoadLibraryExA("PhysXFoundation_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXGpu_64.dll", NULL, NULL);
	LoadLibraryExA("PVDRuntime_64.dll", NULL, NULL);
#endif // _DEBUG

}
