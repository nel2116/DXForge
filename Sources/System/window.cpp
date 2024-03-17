#include "window.h"

// グローバル変数
HWND hwnd;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0); // OSに対して「もうこのアプリは終わるんだ」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 既定の処理を行う
}

void CreateMainWindow(int w, int h, bool fullScreen)
{
	// ウィンドウクラスの定義
	WNDCLASS windowClass = {
		CS_HREDRAW | CS_VREDRAW,             // ウィンドウクラスのスタイル
		WindowProcedure,                     // ウィンドウプロシージャのアドレス
		0, 0,                                // 予備
		GetModuleHandle(nullptr),            // このプログラムのインスタンスハンドル
		LoadIcon(nullptr, IDI_APPLICATION),  // 組み込まれているアイコンのリソース名
		LoadCursor(nullptr, IDC_ARROW),      // 組み込まれているカーソルのリソース名
		(HBRUSH)GetStockObject(WHITE_BRUSH), // ウィンドウ背景のブラシのハンドル
		nullptr,                             // メニューのリソース名
		"MainWindow"                             // ウィンドウクラス名
	};

	// ウィンドウクラスの登録
	RegisterClass(&windowClass);

	// ウィンドウの作成
	DWORD style = WS_OVERLAPPEDWINDOW;
	if (fullScreen)
	{
		style = WS_POPUP;
	}
	hwnd = CreateWindow(
		"MainWindow",                   // ウィンドウクラス名
		WINDOW_NAME,                    // タイトルバーの文字列
		style,                          // ウィンドウスタイル
		CW_USEDEFAULT, CW_USEDEFAULT,	// 座標はOSに任せる
		w, h,                           // ウィンドウの幅と高さ
		nullptr,                        // 親ウィンドウのハンドル
		nullptr,                        // メニューのハンドル
		GetModuleHandle(nullptr),       // このプログラムのインスタンスハンドル
		nullptr);                       // WM_CREATEメッセージのlparamパラメータ

	// ウィンドウを表示
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

HWND GetWindowHandle()
{
	return hwnd;
}
