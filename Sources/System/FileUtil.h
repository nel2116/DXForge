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

// wstringに変換
std::wstring StringToWString(const std::string& str);
// stringに変換
std::string WStringToString(const std::wstring& wstr);
// ファイルパスを検索
bool SearchFilePathA(const char* filename, std::string& result);
bool SearchFilePathW(const wchar_t* filename, std::wstring& result);
// ディレクトリパスを削除
std::string RemoveDirectoryPathA(const std::string& path);
std::wstring RemoveDirectoryPathW(const std::wstring& path);
// ディレクトリパスを取得
std::string GetDirectoryPathA(const char* path);
std::wstring GetDirectoryPathW(const wchar_t* path);

#if defined(UNICODE) || defined(_UNICODE)
inline bool SearchFilePath(const wchar_t* filename, std::wstring& result)
{
	return SearchFilePathW(filename, result);
}

inline std::wstring RemoveDirectoryPath(const std::wstring& path)
{
	return RemoveDirectoryPathW(path);
}

inline std::wstring GetDirectoryPath(const wchar_t* path)
{
	return GetDirectoryPathW(path);
}
#else
inline bool SearchFilePath(const char* filename, std::string& result)
{
	return SearchFilePathA(filename, result);
}

inline std::string RemoveDirectoryPath(const std::string& path)
{
	return RemoveDirectoryPathA(path);
}

inline std::string GetDirectoryPath(const char* path)
{
	return GetDirectoryPathA(path);
}
#endif//defined(UNICODE) || defined(_UNICODE)