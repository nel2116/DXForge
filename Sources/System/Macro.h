// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Macro.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/24 15:05 : 作成
// 概要   : マクロ定義
// 更新履歴
// 2024/06/24  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <string>

// ====== マクロ定義 ======
// メモリリーク検出
#if _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif
// メモリの開放
#define SAFE_UNINIT(p) { if(p) { p->Uninit(); delete(p); (p) = nullptr; } }
#define SAFE_DELETE(p) { if(p) { delete(p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }

// ====== プロトタイプ宣言 ======
/// <summary>
/// コンソール画面にフォーマット付きで文字列を出力する
/// </summary>
/// <param name="format">フォーマット</param>
/// <param name="...">可変長引数</param>
/// <remarks>この関数はデバッグ用です。デバッグ時にしか動作しません</remarks>
void DebugOutputFormatString(const char* format, ...);

/// <summary>
/// ファイルパスから親ディレクトリまでのパスを取得する
/// </summary>
/// <param name="path">ファイルパス</param>
/// <returns>ディレクトリパス</returns>
inline std::string GetDirFromPath(const std::string& path)
{
	const std::string::size_type pos = path.find_last_of("/");
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}
