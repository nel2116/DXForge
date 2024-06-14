// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/10
// �쐬�� : �c���~�m��
// �T�v   : �t�@�C���p�X���`�F�b�N���āC���݂���΃t�@�C���p�X��ݒ肷��֐��̒�`
// �X�V����
// 2024/06/10 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#pragma comment(lib, "Shlwapi.lib")
#include <string>
#include <Shlwapi.h>

bool SearchFilePathA(const char* filename, std::string& result);
bool SearchFilePathW(const wchar_t* filename, std::wstring& result);
std::string RemoveDirectoryPathA(const std::string& path);
std::wstring RemoveDirectoryPathW(const std::wstring& path);
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