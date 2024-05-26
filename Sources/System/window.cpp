// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [window.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : �s��
// �T�v   : �E�B���h�E�֘A�̏���
// �X�V���� : 2024/05/25 ���t�@�N�^�����O
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "window.h"

// ====== Window�N���X�̃����o�֐� ======
// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ���b�Z�[�W�ɂ���ď����𕪊�
	switch (msg)
	{
	case WM_DESTROY:	// OS�ɏI����ʒm
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:	// �L�[�������ꂽ
		switch (wp)
		{
		case VK_ESCAPE:	// ESC�L�[�������ꂽ
			DestroyWindow(hwnd);
			break;

		}

	default:
		return DefWindowProc(hwnd, msg, wp, lp);
		break;
	}
	return 0;
}

// �E�B���h�E�̍쐬
bool Window::Create(int width, int height, const string& titleName, const string& className)
{
	// �C���X�^���X�n���h���擾
	HINSTANCE hInstance = GetModuleHandle(0);

	// ====== ���C���E�B���h�E�̍쐬 ======
	// �E�B���h�E�N���X�̒�`
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);			// �\���̂̃T�C�Y
	wc.lpfnWndProc = (WNDPROC)WndProc;		// �E�B���h�E�v���V�[�W��
	wc.lpszClassName = className.c_str();	// �E�B���h�E�N���X��
	wc.hInstance = hInstance;				// �C���X�^���X�n���h��

	// ��`�����N���X�������o�ϐ��Ɋi�[
	m_windowClass = wc;

	// �E�B���h�E�N���X�̓o�^
	if (!RegisterClassEx(&wc)) { return false; }

	RECT rc = { 0, 0, (LONG)width, (LONG)height };	// �E�B���h�E�̃T�C�Y

	// �E�B���h�E�̍쐬
	m_hwnd = CreateWindowEx(
		0,
		className.c_str(),						// �E�B���h�E�N���X��
		titleName.c_str(),						// �^�C�g����
		WS_OVERLAPPEDWINDOW - WS_THICKFRAME,	// �E�B���h�E�X�^�C��
		CW_USEDEFAULT,							// �E�B���h�E��X���W
		CW_USEDEFAULT,							// �E�B���h�E��Y���W
		rc.right - rc.left,						// �E�B���h�E�̕�
		rc.bottom - rc.top,						// �E�B���h�E�̍���
		nullptr,								// �e�E�B���h�E�̃n���h��
		nullptr,								// ���j���[�n���h��
		hInstance,								// �C���X�^���X�n���h��
		this									// ���̑��̃p�����[�^
	);

	// �E�B���h�E�̍쐬�Ɏ��s
	if (!m_hwnd) { return false; }

	// �E�B���h�E�̕\��
	ShowWindow(m_hwnd, SW_SHOW);

	// �E�B���h�E�̍X�V
	UpdateWindow(m_hwnd);

	return true;
}

bool Window::ProcessMessage()
{
	// ���b�Z�[�W�̎擾
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// �I�����b�Z�[�W��������false��Ԃ�
		if (msg.message == WM_QUIT) { return false; }

		// ���b�Z�[�W����
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}


