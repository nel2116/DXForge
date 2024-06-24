// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Macro.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/24 15:05 : �쐬
// �T�v   : �}�N����`
// �X�V����
// 2024/06/24  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <string>

// ====== �}�N����` ======
// ���������[�N���o
#if _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif
// �������̊J��
#define SAFE_UNINIT(p) { if(p) { p->Uninit(); delete(p); (p) = nullptr; } }
#define SAFE_DELETE(p) { if(p) { delete(p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }

// ====== �v���g�^�C�v�錾 ======
/// <summary>
/// �R���\�[����ʂɃt�H�[�}�b�g�t���ŕ�������o�͂���
/// </summary>
/// <param name="format">�t�H�[�}�b�g</param>
/// <param name="...">�ϒ�����</param>
/// <remarks>���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���</remarks>
void DebugOutputFormatString(const char* format, ...);

/// <summary>
/// �t�@�C���p�X����e�f�B���N�g���܂ł̃p�X���擾����
/// </summary>
/// <param name="path">�t�@�C���p�X</param>
/// <returns>�f�B���N�g���p�X</returns>
inline std::string GetDirFromPath(const std::string& path)
{
	const std::string::size_type pos = path.find_last_of("/");
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}
