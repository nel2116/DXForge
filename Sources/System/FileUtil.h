// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/10
// 作成者 : 田中ミノル
// 概要   : ファイルパスをチェックして，存在すればファイルパスを設定する関数の定義
// 更新履歴
// 2024/06/10 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#pragma comment(lib, "Shlwapi.lib")
#include <string>
#include <Shlwapi.h>

bool SearchFilePath(const wchar_t* fileName, std::wstring& result);
