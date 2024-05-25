// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/05/21 17:00
// 作成者 : 田中ミノル
// 概要   : entry point
// 更新履歴
// 2024/05/25 pause()関数を追加
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include <stdio.h>
#include <Application/Application.h>

// ====== プロトタイプ宣言 ======
void pause(void);	// [Enter]キーの入力待ち

// エントリーポイント
#ifndef _DEBUG
int main(void)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// メモリリーク検出
	auto& app = Application::Instance();	// シングルトンのインスタンスを取得
	if (!app.Init()) { return -1; }

	app.Run();		// アプリケーションの実行
	app.Uninit();	// アプリケーションの終了処理
	pause();		// [Enter]キーの入力待ち
	return 0;
}

/// <summary>
/// [Enter]キーの入力待ち
/// </summary>
void pause(void)
{
	printf("エンターキーを入力してください。");
	rewind(stdin);
	(void)getchar();	// 戻り値を無視
}
