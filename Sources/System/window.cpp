#include "window.h"

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

void CreateMainWindow(HWND& hwnd, WNDCLASSEX& windowClass)
{
	HINSTANCE hInst = GetModuleHandle(nullptr); // インスタンスハンドルを取得
	// ウィンドウクラス生成＆登録
	windowClass.cbSize = sizeof(WNDCLASSEX); // 構造体のサイズ
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure; // ウィンドウプロシージャのアドレス
	windowClass.lpszClassName = "DXForgeTest"; // ウィンドウクラス名
	windowClass.hInstance = GetModuleHandle(0); // ハンドルの取得
	RegisterClassEx(&windowClass); // OSに登録

	RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // ウィンドウサイズを決める
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // ウィンドウのサイズを調整
	// ウィンドウ生成
	hwnd = CreateWindow(
		windowClass.lpszClassName, // ウィンドウクラス名
		WINDOW_NAME, // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウです
		CW_USEDEFAULT,//表示X座標はOSにお任せします
		CW_USEDEFAULT,//表示Y座標はOSにお任せします
		wrc.right - wrc.left,//ウィンドウ幅
		wrc.bottom - wrc.top,//ウィンドウ高
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		windowClass.hInstance,//呼び出しアプリケーションハンドル
		nullptr);//追加パラメータ

}
