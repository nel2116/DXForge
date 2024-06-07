// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/05/21 17:00
// �쐬�� : �c���~�m��
// �T�v   : entry point
// �X�V����
// 2024/05/25 pause()�֐���ǉ�
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include <stdio.h>
#include <Application/Application.h>


#if defined(_DEBUG) || defined(DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif	// _DEBUG || DEBUG


// ====== �v���g�^�C�v�錾 ======
void pause(void);	// [Enter]�L�[�̓��͑҂�

// �G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// ���������[�N���o
	// _CrtSetBreakAlloc(0);	// ���������[�N�������Ƀu���[�N�|�C���g��ݒ�

#endif	// _DEBUG || DEBUG

	// �A�v���P�[�V�����̏�����
	auto& app = Application::Instance();	// �V���O���g���̃C���X�^���X���擾
	if (!app.Init()) { return -1; }

	app.Run();		// �A�v���P�[�V�����̎��s
	app.Uninit();	// �A�v���P�[�V�����̏I������
	pause();		// [Enter]�L�[�̓��͑҂�
	return 0;
}

/// <summary>
/// [Enter]�L�[�̓��͑҂�
/// </summary>
void pause(void)
{
	printf("�G���^�[�L�[����͂��Ă��������B");
	rewind(stdin);
	(void)getchar();	// �߂�l�𖳎�
}
