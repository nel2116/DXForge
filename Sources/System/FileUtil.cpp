// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/10
// �쐬�� : �c���~�m��
// �T�v   : �t�@�C���p�X���`�F�b�N���āC���݂���΃t�@�C���p�X��ݒ肷��֐��̎���
// �X�V����
// 2024/06/10 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "FileUtil.h"

#define PATH_MAX 520

namespace
{
	// �������u��
	std::string Replace
	(
		const std::string& input,
		std::string         pattern,
		std::string         replace)
	{
		std::string result = input;
		auto pos = result.find(pattern);

		while (pos != std::string::npos)
		{
			result.replace(pos, pattern.length(), replace);
			pos = result.find(pattern, pos + replace.length());
		}

		return result;
	}

	std::wstring Replace
	(
		const std::wstring& input,
		std::wstring         pattern,
		std::wstring         replace)
	{
		std::wstring result = input;
		auto pos = result.find(pattern);

		while (pos != std::wstring::npos)
		{
			result.replace(pos, pattern.length(), replace);
			pos = result.find(pattern, pos + replace.length());
		}

		return result;
	}
}// namespace


bool SearchFilePathA(const char* filename, std::string& result)
{
	if (filename == nullptr)
	{
		return false;
	}

	if (strcmp(filename, " ") == 0 || strcmp(filename, "") == 0)
	{
		return false;
	}

	char exePath[520] = { 0 };
	memset(exePath, '\0', sizeof(exePath));
	GetModuleFileNameA(nullptr, exePath, 520);
	PathRemoveFileSpecA(exePath);

	char dstPath[520] = { 0 };
	memset(dstPath, '\0', sizeof(dstPath));

	strcpy_s(dstPath, filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "..\\%s", filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "..\\..\\%s", filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "\\Assets\\%s", filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "%s\\%s", exePath, filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "%s\\..\\%s", exePath, filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "%s\\..\\..\\%s", exePath, filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	sprintf_s(dstPath, "%s\\Assets\\%s", exePath, filename);
	if (PathFileExistsA(dstPath) == TRUE)
	{
		result = Replace(dstPath, "\\", "/");
		return true;
	}

	return false;
}

bool SearchFilePathW(const wchar_t* filename, std::wstring& result)
{
	if (filename == nullptr)
	{
		return false;
	}

	if (wcscmp(filename, L" ") == 0 || wcscmp(filename, L"") == 0)
	{
		return false;
	}

	wchar_t exePath[520];
	memset(exePath, L'\0', sizeof(exePath));
	GetModuleFileNameW(nullptr, exePath, 520);
	PathRemoveFileSpecW(exePath);

	wchar_t dstPath[520];
	memset(dstPath, L'\0', sizeof(exePath));

	wcscpy_s(dstPath, filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"..\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"..\\..\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"\\Assets\\%s", filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"%s\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"%s\\..\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"%s\\..\\..\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	swprintf_s(dstPath, L"%s\\Assets\\%s", exePath, filename);
	if (PathFileExistsW(dstPath) == TRUE)
	{
		result = Replace(dstPath, L"\\", L"/");
		return true;
	}

	return false;
}

// �f�B���N�g���p�X���폜���A�t�@�C������Ԃ�
std::string RemoveDirectoryPathA(const std::string& path)
{
	auto temp = Replace(path, "\\", "/");
	auto pos = temp.rfind('/');

	if (pos != std::string::npos)
	{
		return temp.substr(pos + 1);
	}

	return path;
}

// �f�B���N�g���p�X���폜���A�t�@�C������Ԃ�
std::wstring RemoveDirectoryPathW(const std::wstring& path)
{
	auto temp = Replace(path, L"\\", L"/");
	auto pos = temp.rfind(L'/');

	if (pos != std::wstring::npos)
	{
		return temp.substr(pos + 1);
	}

	return path;
}

// �t�@�C���p�X����f�B���N�g�������擾����
std::string GetDirectoryPathA(const char* filePath)
{
	std::string path = Replace(filePath, "\\", "/");
	size_t idx = path.find_last_of("/");
	if (idx != std::string::npos)
	{
		return path.substr(0, idx + 1);
	}

	return std::string();
}

// �t�@�C���p�X����f�B���N�g�������擾����
std::wstring GetDirectoryPathW(const wchar_t* filePath)
{
	std::wstring path = Replace(filePath, L"\\", L"/");
	size_t idx = path.find_last_of(L"/");
	if (idx != std::wstring::npos)
	{
		return path.substr(0, idx + 1);
	}

	return std::wstring();
}