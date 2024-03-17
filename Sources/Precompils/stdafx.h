#pragma once
// ====== インクルード部 ======
// windows標準ヘッダー
#include <iostream>
#include <crtdbg.h>
#include <vector>
#include <string>
// DirectX12APIヘッダー

// 自作ヘッダー
#include <System/window.h>

// ====== 名前空間 ======
using namespace std;

// ====== 定数定義 ======
// 円周率
const float PI = 3.1415926f;

// ====== マクロ定義 ======
// メモリリーク検出
#if _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif
// メモリの開放
#define SAFE_DELETE(p) { if(p) { delete(p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

// ====== プロトタイプ宣言 ======
/// <summary>
/// コンソール画面にフォーマット付きで文字列を出力する
/// </summary>
/// <param name="format">フォーマット</param>
/// <param name="...">可変長引数</param>
/// <remarks>この関数はデバッグ用です。デバッグ時にしか動作しません</remarks>
void DebugOutputFormatString(const char* format, ...);
