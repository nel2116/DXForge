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

bool SearchFilePath(const wchar_t* fileName, std::wstring& result)
{
	// �t�@�C�������󕶎����ǂ������`�F�b�N
	if (fileName == nullptr) { return false; }
	if (wcscmp(fileName, L" ") == 0 || wcscmp(fileName, L"") == 0) { return false; }

	// ���s�t�@�C���̃p�X���擾
	wchar_t exePath[PATH_MAX] = {};
	// ���s�t�@�C���̃p�X���擾
	GetModuleFileNameW(nullptr, exePath, PATH_MAX);
	exePath[PATH_MAX - 1] = L'\0'; // null�I�[��
	// �t�@�C��������菜��
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
