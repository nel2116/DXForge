// ====== インクルード部 ======
#include <DirectX/renderer.h>

// ====== メイン関数 ======
#ifdef _DEBUG
int main(void)
{
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#endif
	// メモリリークチェック
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ----- 初期化処理 -----
	// ウィンドウ生成
	CreateMainWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false);

	// レンダラー生成
	Renderer* renderer = NEW Renderer();
	if (!renderer->Init()) { SAFE_DELETE(renderer); return 0; }





	// メッセージループ
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) { break; }
		// ゲームの処理
		renderer->Update();
		renderer->Draw();
	}


	// DebugOutputFormatString("Show window test");

	// ----- 終了処理 -----
	// ウィンドウクラスの登録を解除
	UnregisterClass("MainWindow", GetModuleHandle(nullptr));
	getchar();
	// レンダラーの解放
	SAFE_DELETE(renderer);
	return 0;
}