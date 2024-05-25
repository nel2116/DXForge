// �쐬�� : 2024/05/21 17:00
// �쐬�� : �c���~�m��
// �T�v   : entry point
#include <Application/Application.h>

#ifndef _DEBUG
int main(void)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// ���������[�N���o
	auto& app = Application::Instance();	// �V���O���g���̃C���X�^���X���擾
	if (!app.Init()) { return -1; }

	app.Run();
	app.Terminate();
	return 0;
}
