// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/10
// 作成者 : 田中ミノル
// 概要   : ファイルパスをチェックして，存在すればファイルパスを設定する関数の実装
// 更新履歴
// 2024/06/10 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "FileUtil.h"

#define PATH_MAX 520

bool SearchFilePath(const wchar_t* fileName, std::wstring& result)
{
	// ファイル名が空文字かどうかをチェック
	if (fileName == nullptr) { return false; }
	if (wcscmp(fileName, L" ") == 0 || wcscmp(fileName, L"") == 0) { return false; }

	// 実行ファイルのパスを取得
	wchar_t exePath[PATH_MAX] = {};
	// 実行ファイルのパスを取得
	GetModuleFileNameW(nullptr, exePath, PATH_MAX);
	exePath[PATH_MAX - 1] = L'\0'; // null終端化
	// ファイル名を取り除く
	PathRemoveFileSpecW(exePath);

	wchar_t dstPath[PATH_MAX] = {};

	wcscpy_s(dstPath, fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"..\\%s", fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"..\\..\\%s", fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"\\Assets\\%s", fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"%s\\%s", exePath, fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"%s\\..\\%s", exePath, fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"%s\\..\\..\\%s", exePath, fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	swprintf_s(dstPath, L"%s\\Assets\\%s", exePath, fileName);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = dstPath;
		return true;
	}

	return false;
}
