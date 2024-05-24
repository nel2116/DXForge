#include "window.h"

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

void CreateMainWindow(HWND& hwnd, WNDCLASSEX& windowClass)
{
	HINSTANCE hInst = GetModuleHandle(nullptr); // �C���X�^���X�n���h�����擾
	// �E�B���h�E�N���X�������o�^
	windowClass.cbSize = sizeof(WNDCLASSEX); // �\���̂̃T�C�Y
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure; // �E�B���h�E�v���V�[�W���̃A�h���X
	windowClass.lpszClassName = "DXForgeTest"; // �E�B���h�E�N���X��
	windowClass.hInstance = GetModuleHandle(0); // �n���h���̎擾
	RegisterClassEx(&windowClass); // OS�ɓo�^

	RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // �E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �E�B���h�E�̃T�C�Y�𒲐�
	// �E�B���h�E����
	hwnd = CreateWindow(
		windowClass.lpszClassName, // �E�B���h�E�N���X��
		WINDOW_NAME, // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E�ł�
		CW_USEDEFAULT,//�\��X���W��OS�ɂ��C�����܂�
		CW_USEDEFAULT,//�\��Y���W��OS�ɂ��C�����܂�
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		windowClass.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);//�ǉ��p�����[�^

}
