// 作成日 : 2024/05/21 17:00
// 作成者 : 田中ミノル
// 概要   : entry point
#include <Application/Application.h>

#ifndef _DEBUG
int main(void)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// メモリリーク検出
	auto& app = Application::Instance();	// シングルトンのインスタンスを取得
	if (!app.Init()) { return -1; }

	app.Run();
	app.Terminate();
	return 0;
}
