// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [window.cpp]
// 作成者 : 田中ミノル
// 作成日 : 不明
// 概要   : ウィンドウ関連の処理
// 更新履歴 : 2024/05/25 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "window.h"

// ====== Windowクラスのメンバ関数 ======
// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// メッセージによって処理を分岐
	switch (msg)
	{
	case WM_DESTROY:	// OSに終了を通知
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:	// キーが押された
		switch (wp)
		{
		case VK_ESCAPE:	// ESCキーが押された
			DestroyWindow(hwnd);
			break;

		}

	default:
		return DefWindowProc(hwnd, msg, wp, lp);
		break;
	}
	return 0;
}

// ウィンドウの作成
bool Window::Create(int width, int height, const string& titleName, const string& className)
{
	// インスタンスハンドル取得
	HINSTANCE hInstance = GetModuleHandle(0);

	// ====== メインウィンドウの作成 ======
	// ウィンドウクラスの定義
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);			// 構造体のサイズ
	wc.lpfnWndProc = (WNDPROC)WndProc;		// ウィンドウプロシージャ
	wc.lpszClassName = className.c_str();	// ウィンドウクラス名
	wc.hInstance = hInstance;				// インスタンスハンドル

	// 定義したクラスをメンバ変数に格納
	m_windowClass = wc;

	// ウィンドウクラスの登録
	if (!RegisterClassEx(&wc)) { return false; }

	RECT rc = { 0, 0, (LONG)width, (LONG)height };	// ウィンドウのサイズ

	// ウィンドウの作成
	m_hwnd = CreateWindowEx(
		0,
		className.c_str(),						// ウィンドウクラス名
		titleName.c_str(),						// タイトル名
		WS_OVERLAPPEDWINDOW - WS_THICKFRAME,	// ウィンドウスタイル
		CW_USEDEFAULT,							// ウィンドウのX座標
		CW_USEDEFAULT,							// ウィンドウのY座標
		rc.right - rc.left,						// ウィンドウの幅
		rc.bottom - rc.top,						// ウィンドウの高さ
		nullptr,								// 親ウィンドウのハンドル
		nullptr,								// メニューハンドル
		hInstance,								// インスタンスハンドル
		this									// その他のパラメータ
	);

	// ウィンドウの作成に失敗
	if (!m_hwnd) { return false; }

	// ウィンドウの表示
	ShowWindow(m_hwnd, SW_SHOW);

	// ウィンドウの更新
	UpdateWindow(m_hwnd);

	return true;
}

bool Window::ProcessMessage()
{
	// メッセージの取得
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// 終了メッセージが来たらfalseを返す
		if (msg.message == WM_QUIT) { return false; }

		// メッセージ処理
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}


