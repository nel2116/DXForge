//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Application.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �A�v���P�[�V�����N���X�̎���
// �X�V���� : 2024/05/25 �쐬
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Application.h"
#include <Manager/ActorManager.h>

// ====== �����o�֐� ======
bool Application::Init()
{
	SetDirectoryAndLoadDll();	// .dll�̃f�B���N�g���̃Z�b�g�ƃ��[�h���s��

	// COM�̏�����
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		MessageBox(nullptr, "COM�̏������Ɏ��s���܂����B", "�G���[", MB_OK);
		assert(0 && "COM�̏������Ɏ��s���܂����B");
		return false;
	}

	if (!m_window.Create(WINDOW_WIDTH, WINDOW_HEIGHT, CLASS_NAME, WINDOW_NAME))
	{
		assert(0 && "�E�B���h�E�̍쐬�Ɏ��s���܂����B");
		return false;
	}

	if (!RENDERER.Init(&m_window))
	{
		assert(0 && "Renderer�̏������Ɏ��s���܂����B");
		return false;
	}

	// ====== Manager�̏����� ======
	ACTOR_MANAGER.Init();

	if (!m_Polygon.Init())
	{
		assert(0 && "�|���S���̏������Ɏ��s���܂����B");
		return false;
	}

	// �^�C�}�[�̏�����
	timeBeginPeriod(1);
	m_dwExecLastTime = timeGetTime();
	m_dwCurrentTime = m_dwExecLastTime;
	m_dwFrameCount = 0;
	m_dwLsatFPSTime = m_dwExecLastTime;
	m_fFPS = 0.0f;
	return true;
}

void Application::Run()
{
	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }

		m_dwCurrentTime = timeGetTime();
		if ((m_dwCurrentTime - m_dwExecLastTime) >= FRAME_TIME)
		{
			m_dwExecLastTime = m_dwCurrentTime;
			// ====== �����ɃQ�[���̏��������� ======
			// ------ �X�V���� ------
			ACTOR_MANAGER.Update();

			// ------ �`�揈�� ------
			RENDERER.Begin();

			// --- �ȉ��Q�[���̕`�揈��
			ACTOR_MANAGER.Draw();
			m_Polygon.Draw();

			// --- �����܂ŃQ�[���̕`�揈��

			// ----- �`��I�� -----
			RENDERER.End();
			// ========================================
			m_dwFrameCount++;
		}
	}
}

void Application::Uninit()
{
	// ====== �I������ ======
	m_Polygon.Uninit();	// �|���S���̏I������

	// ----- Manager�̏I������ -----
	ACTOR_MANAGER.Uninit();	// ActorManager�̏I������

	// ----- �V�X�e���̏I������ -----
	RENDERER.Uninit();	// Renderer�̏I������
	timeEndPeriod(1);	// �^�C�}�[�̏I������
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// �E�B���h�E�N���X�̓o�^����
	CoUninitialize();	// COM�̏I������
}

void Application::SetDirectoryAndLoadDll()
{
#ifdef _DEBUG
	SetDllDirectory("Library/assimp/build/lib/Debug");
	LoadLibraryExA("assimp-vc143-mtd.dll", NULL, NULL);
#else
	SetDllDirectory("Library/assimp/build/lib/Release");
	LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif // _DEBUG

}
