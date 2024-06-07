// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [window.cpp]
// 作成者 : 田中ミノル
// 作成日 : 不明
// 概要   : ウィンドウ関連の処理
// 更新履歴 : 2024/05/25 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "window.h"

// メンバ変数の初期化
int Window::m_width = 0;
int Window::m_height = 0;
HWND Window::m_hwnd = nullptr;

// ====== Windowクラスのメンバ関数 ======
// ImGuiのウィンドウプロシージャ
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))return true;

	// メッセージによって処理を分岐
	switch (msg)
	{
	case WM_CLOSE:		// ウィンドウが閉じられた
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:	// OSに終了を通知
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:	// キーが押された
		switch (wp)
		{
		case VK_ESCAPE:	// ESCキーが押された
			PostQuitMessage(0);
			break;
		}
		break;

	default:
		return DefWindowProc(hwnd, msg, wp, lp);	// 未処理のメッセージをデフォルト処理
	}
	return 0;
}

// ウィンドウの作成
bool Window::Create(int width, int height, const string& titleName, const string& className)
{
	// ウィンドウのサイズをメンバ変数に格納
	m_width = width;
	m_height = height;

	// インスタンスハンドル取得
	HINSTANCE hInstance = GetModuleHandle(0);

	// ====== メインウィンドウの作成 ======
	// ウィンドウクラスの定義
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);			// 構造体のサイズ
	wc.lpfnWndProc = WndProc;				// ウィンドウプロシージャ
	wc.lpszClassName = className.c_str();	// ウィンドウクラス名
	wc.hInstance = hInstance;				// インスタンスハンドル

	// 定義したクラスをメンバ変数に格納
	m_windowClass = wc;

	// ウィンドウクラスの登録
	if (!RegisterClassEx(&wc)) { return false; }

	// ウィンドウの作成
	m_hwnd = CreateWindowEx(
		0,
		className.c_str(),						// ウィンドウクラス名
		titleName.c_str(),						// タイトル名
		WINDOW_STYLE,							// ウィンドウスタイル
		CW_USEDEFAULT,							// ウィンドウのX座標
		CW_USEDEFAULT,							// ウィンドウのY座標
		CW_USEDEFAULT,							// ウィンドウの幅
		CW_USEDEFAULT,							// ウィンドウの高さ
		nullptr,								// 親ウィンドウのハンドル
		nullptr,								// メニューハンドル
		hInstance,								// インスタンスハンドル
		this									// その他のパラメータ
	);

	// ウィンドウの作成に失敗
	if (!m_hwnd) { return false; }

	// ウィンドウサイズの変更
	SetClientSize(width, height);

	// ウィンドウの表示
	ShowWindow(m_hwnd, SW_SHOW);

	// ウィンドウの更新
	UpdateWindow(m_hwnd);

	// ウィンドウのフォーカスを設定
	SetFocus(m_hwnd);

	return true;
}

bool Window::ProcessMessage()
{
	// メッセージの取得
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// 終了メッセージが来たらfalseを返す
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		// メッセージ処理
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

VOID Window::SetClientSize(LONG sx, LONG sy)
{
	RECT rc1;
	RECT rc2;

	GetWindowRect(m_hwnd, &rc1);
	GetClientRect(m_hwnd, &rc2);
	sx += ((rc1.right - rc1.left) - (rc2.right - rc2.left));
	sy += ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top));
	SetWindowPos(m_hwnd, NULL, 0, 0, sx, sy, (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));
}


