// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [window.h]
// �쐬�� : �c���~�m��
// �쐬�� : �s��
// �T�v   : �E�B���h�E�֘A�̏���
// �X�V���� : 2024/05/25 ���t�@�N�^�����O
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>

// ====== �萔��` ======
// �E�B���h�E�̃^�C�g��
#define WINDOW_NAME "DXForge"
// �N���X�̖��O
#define CLASS_NAME "DXForgeWindow"
// 
#define WINDOW_STYLE (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX))
// �E�B���h�E�̕�
const int WINDOW_WIDTH = 1280;
// �E�B���h�E�̍���
const int WINDOW_HEIGHT = 720;
// �E�B���h�E�̃A�X�y�N�g��
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// ====== �N���X��` ======
class Window
{
public:		// �����o�֐�
	/// <summary>
	/// �E�B���h�E�̍쐬
	/// </summary>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="titleName">�^�C�g����(�L���v�V������)</param>
	/// <param name="className">�N���X��</param>
	/// <returns>�쐬�ɐ���������true</returns>
	bool Create(int width, int height, const std::string& titleName, const std::string& className);

	/// <summary>
	/// �E�B���h�E���b�Z�[�W����
	/// </summary>
	/// <returns>�I�����b�Z�[�W��������false</returns>
	bool ProcessMessage();

	HWND GetHwnd() const { return m_hwnd; }

	HINSTANCE GetHinstance() const { return m_windowClass.hInstance; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:	// �v���C�x�[�g�֐�
	static VOID SetClientSize(LONG sx, LONG sy);

private:	// �����o�ϐ�
	static HWND m_hwnd;
	WNDCLASSEX m_windowClass;
	static int m_width;
	static int m_height;
};
