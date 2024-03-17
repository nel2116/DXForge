#pragma once
// ====== インクルード部 ======
#include <windows.h>

// ====== 定数定義 ======
// ウィンドウのタイトル
#define WINDOW_NAME "DXForge"

// ウィンドウの幅
const int WINDOW_WIDTH = 1280;
// ウィンドウの高さ
const int WINDOW_HEIGHT = 720;
// ウィンドウのアスペクト比
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// ====== プロトタイプ宣言 ======
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void CreateMainWindow(int w, int h, bool fullScreen);
HWND GetWindowHandle();
