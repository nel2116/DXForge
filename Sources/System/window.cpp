#include "window.h"

// �O���[�o���ϐ�
HWND hwnd;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0); // OS�ɑ΂��āu�������̃A�v���͏I���񂾁v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // ����̏������s��
}

void CreateMainWindow(int w, int h, bool fullScreen)
{
	// �E�B���h�E�N���X�̒�`
	WNDCLASS windowClass = {
		CS_HREDRAW | CS_VREDRAW,             // �E�B���h�E�N���X�̃X�^�C��
		WindowProcedure,                     // �E�B���h�E�v���V�[�W���̃A�h���X
		0, 0,                                // �\��
		GetModuleHandle(nullptr),            // ���̃v���O�����̃C���X�^���X�n���h��
		LoadIcon(nullptr, IDI_APPLICATION),  // �g�ݍ��܂�Ă���A�C�R���̃��\�[�X��
		LoadCursor(nullptr, IDC_ARROW),      // �g�ݍ��܂�Ă���J�[�\���̃��\�[�X��
		(HBRUSH)GetStockObject(WHITE_BRUSH), // �E�B���h�E�w�i�̃u���V�̃n���h��
		nullptr,                             // ���j���[�̃��\�[�X��
		"MainWindow"                             // �E�B���h�E�N���X��
	};

	// �E�B���h�E�N���X�̓o�^
	RegisterClass(&windowClass);

	// �E�B���h�E�̍쐬
	DWORD style = WS_OVERLAPPEDWINDOW;
	if (fullScreen)
	{
		style = WS_POPUP;
	}
	hwnd = CreateWindow(
		"MainWindow",                   // �E�B���h�E�N���X��
		WINDOW_NAME,                    // �^�C�g���o�[�̕�����
		style,                          // �E�B���h�E�X�^�C��
		CW_USEDEFAULT, CW_USEDEFAULT,	// ���W��OS�ɔC����
		w, h,                           // �E�B���h�E�̕��ƍ���
		nullptr,                        // �e�E�B���h�E�̃n���h��
		nullptr,                        // ���j���[�̃n���h��
		GetModuleHandle(nullptr),       // ���̃v���O�����̃C���X�^���X�n���h��
		nullptr);                       // WM_CREATE���b�Z�[�W��lparam�p�����[�^

	// �E�B���h�E��\��
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

HWND GetWindowHandle()
{
	return hwnd;
}
