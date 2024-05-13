// ====== �C���N���[�h�� ======
#include <DirectX/renderer.h>

// ====== ���C���֐� ======
#ifdef _DEBUG
int main(void)
{
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#endif
	// ���������[�N�`�F�b�N
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ----- ���������� -----
	// �E�B���h�E����
	CreateMainWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false);

	// �����_���[����
	Renderer* renderer = NEW Renderer();
	if (!renderer->Init()) { SAFE_DELETE(renderer); return 0; }





	// ���b�Z�[�W���[�v
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) { break; }
		// �Q�[���̏���
		renderer->Update();
		renderer->Draw();
	}


	// DebugOutputFormatString("Show window test");

	// ----- �I������ -----
	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass("MainWindow", GetModuleHandle(nullptr));
	getchar();
	// �����_���[�̉��
	SAFE_DELETE(renderer);
	return 0;
}