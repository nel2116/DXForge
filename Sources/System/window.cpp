// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [window.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : �s��
// �T�v   : �E�B���h�E�֘A�̏���
// �X�V���� : 2024/05/25 ���t�@�N�^�����O
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "window.h"

// �����o�ϐ��̏�����
int Window::m_width = 0;
int Window::m_height = 0;
HWND Window::m_hwnd = nullptr;

// ====== Window�N���X�̃����o�֐� ======
// ImGui�̃E�B���h�E�v���V�[�W��
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))return true;

	// ���b�Z�[�W�ɂ���ď����𕪊�
	switch (msg)
	{
	case WM_CLOSE:		// �E�B���h�E������ꂽ
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:	// OS�ɏI����ʒm
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:	// �L�[�������ꂽ
		switch (wp)
		{
		case VK_ESCAPE:	// ESC�L�[�������ꂽ
			PostQuitMessage(0);
			break;
		}
		break;

	default:
		return DefWindowProc(hwnd, msg, wp, lp);	// �������̃��b�Z�[�W���f�t�H���g����
	}
	return 0;
}

// �E�B���h�E�̍쐬
bool Window::Create(int width, int height, const string& titleName, const string& className)
{
	// �E�B���h�E�̃T�C�Y�������o�ϐ��Ɋi�[
	m_width = width;
	m_height = height;

	// �C���X�^���X�n���h���擾
	HINSTANCE hInstance = GetModuleHandle(0);

	// ====== ���C���E�B���h�E�̍쐬 ======
	// �E�B���h�E�N���X�̒�`
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);			// �\���̂̃T�C�Y
	wc.lpfnWndProc = WndProc;				// �E�B���h�E�v���V�[�W��
	wc.lpszClassName = className.c_str();	// �E�B���h�E�N���X��
	wc.hInstance = hInstance;				// �C���X�^���X�n���h��

	// ��`�����N���X�������o�ϐ��Ɋi�[
	m_windowClass = wc;

	// �E�B���h�E�N���X�̓o�^
	if (!RegisterClassEx(&wc)) { return false; }

	// �E�B���h�E�̍쐬
	m_hwnd = CreateWindowEx(
		0,
		className.c_str(),						// �E�B���h�E�N���X��
		titleName.c_str(),						// �^�C�g����
		WINDOW_STYLE,							// �E�B���h�E�X�^�C��
		CW_USEDEFAULT,							// �E�B���h�E��X���W
		CW_USEDEFAULT,							// �E�B���h�E��Y���W
		CW_USEDEFAULT,							// �E�B���h�E�̕�
		CW_USEDEFAULT,							// �E�B���h�E�̍���
		nullptr,								// �e�E�B���h�E�̃n���h��
		nullptr,								// ���j���[�n���h��
		hInstance,								// �C���X�^���X�n���h��
		this									// ���̑��̃p�����[�^
	);

	// �E�B���h�E�̍쐬�Ɏ��s
	if (!m_hwnd) { return false; }

	// �E�B���h�E�T�C�Y�̕ύX
	SetClientSize(width, height);

	// �E�B���h�E�̕\��
	ShowWindow(m_hwnd, SW_SHOW);

	// �E�B���h�E�̍X�V
	UpdateWindow(m_hwnd);

	// �E�B���h�E�̃t�H�[�J�X��ݒ�
	SetFocus(m_hwnd);

	return true;
}

bool Window::ProcessMessage()
{
	// ���b�Z�[�W�̎擾
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// �I�����b�Z�[�W��������false��Ԃ�
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		// ���b�Z�[�W����
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

VOID Window::SetClientSize(LONG sx, LONG sy)
{
	RECT rc1;
	RECT rc2;

	GetWindowRect(m_hwnd, &rc1);
	GetClientRect(m_hwnd, &rc2);
	sx += ((rc1.right - rc1.left) - (rc2.right - rc2.left));
	sy += ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top));
	SetWindowPos(m_hwnd, NULL, 0, 0, sx, sy, (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));
}


