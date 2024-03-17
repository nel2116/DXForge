#pragma once
// ====== �C���N���[�h�� ======
// windows�W���w�b�_�[
#include <iostream>
#include <crtdbg.h>
#include <vector>
#include <string>
// DirectX12API�w�b�_�[

// ����w�b�_�[
#include <System/window.h>

// ====== ���O��� ======
using namespace std;

// ====== �萔��` ======
// �~����
const float PI = 3.1415926f;

// ====== �}�N����` ======
// ���������[�N���o
#if _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif
// �������̊J��
#define SAFE_DELETE(p) { if(p) { delete(p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

// ====== �v���g�^�C�v�錾 ======
/// <summary>
/// �R���\�[����ʂɃt�H�[�}�b�g�t���ŕ�������o�͂���
/// </summary>
/// <param name="format">�t�H�[�}�b�g</param>
/// <param name="...">�ϒ�����</param>
/// <remarks>���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���</remarks>
void DebugOutputFormatString(const char* format, ...);
