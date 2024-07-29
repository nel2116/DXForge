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
#include <System/Input.h>

using namespace DirectX::SimpleMath;

// ====== �����o�֐� ======
bool Application::Init()
{
	SetDirectoryAndLoadDll();	// .dll�̃f�B���N�g���̃Z�b�g�ƃ��[�h���s��

	// COM�̏�����
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		ELOG("[App.cpp]Error : Line56 : COM�̏������Ɏ��s���܂����B");
		return false;
	}

	if (!m_window.Create(WINDOW_WIDTH, WINDOW_HEIGHT, CLASS_NAME, WINDOW_NAME))
	{
		ELOG("[App.cpp]Error : Line62 : �E�B���h�E�̐����Ɏ��s���܂����B");
		return false;
	}

	if (!RENDERER.Init(&m_window))
	{
		ELOG("[App.cpp]Error : Line68 : Renderer�̏������Ɏ��s���܂����B");
		return false;
	}

	auto hr = InitInput();
	if (FAILED(hr))
	{
		ELOG("[App.cpp]Error : Line75 : ���͂̏������Ɏ��s���܂����B%s", hr);
		return false;
	}

	// TimerManager�̏�����
	TIMER_MANAGER.Init();

	// ShaderManager�̏�����
	SHADER_MANAGER.Init();

	// SceneManager�̏�����
	SCENE_MANAGER.Init();

	return true;
}

void Application::Run()
{
	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }

		// �^�C�}�[�̍X�V
		TIMER_MANAGER.Update();

		if (TIMER_MANAGER.IsFrameGo())
		{
			// ====== �����ɃQ�[���̏��������� ======
			// ------ �X�V���� ------
			// ���͂̍X�V
			UpdateInput();

			// SceneManager�̍X�V
			SCENE_MANAGER.Update();
			RENDERER.Update();

			// ------ �`�揈�� ------
			RENDERER.Begin();

			// �V�[���̕`��
			SCENE_MANAGER.Draw();

			// --- �����܂ŃQ�[���̕`�揈��

			// ----- �`��I�� -----
			RENDERER.End();

			// ========================================
			TIMER_MANAGER.UpdateFrame();
		}
	}
}

void Application::Uninit()
{
	// ====== �I������ ======
	RENDERER.WaitGpu();

	// �}�l�[�W���[�̉��
	SCENE_MANAGER.Uninit();
	SHADER_MANAGER.Uninit();
	TIMER_MANAGER.Uninit();

	// ----- �V�X�e���̏I������ -----
	UninitInput();	// ���͂̏I������
	RENDERER.Uninit();	// Renderer�̏I������
	timeEndPeriod(1);	// �^�C�}�[�̏I������
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// �E�B���h�E�N���X�̓o�^����
	CoUninitialize();	// COM�̏I������
}

void Application::SetDirectoryAndLoadDll()
{
#ifdef _DEBUG
	// �f�o�b�O�r���h�̏ꍇ�̓f�o�b�O�p��DLL��ǂݍ���
	// Assimp��DLL��ǂݍ���
	// DLL�̃f�B���N�g����ݒ�
	SetDllDirectory("Library/assimp/build/lib/Debug");
	// DLL�̓ǂݍ���
	LoadLibraryExA("assimp-vc143-mtd.dll", NULL, NULL);

	// PhysX��DLL��ǂݍ���
	// DLL�̃f�B���N�g����ݒ�
	SetDllDirectory("Library/PhysX/bin/Debug");
	// DLL�̓ǂݍ���
	LoadLibraryExA("freeglutd.dll", NULL, NULL);
	LoadLibraryExA("PhysX_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCommon_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCooking_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXDevice64.dll", NULL, NULL);
	LoadLibraryExA("PhysXFoundation_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXGpu_64.dll", NULL, NULL);
	LoadLibraryExA("PVDRuntime_64.dll", NULL, NULL);
#else
	// �����[�X�r���h�̏ꍇ�̓����[�X�p��DLL��ǂݍ���
	// Assimp��DLL��ǂݍ���
	// DLL�̃f�B���N�g����ݒ�
	SetDllDirectory("Library/assimp/build/lib/Release");
	// DLL�̓ǂݍ���
	LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);

	// PhysX��DLL��ǂݍ���
	// DLL�̃f�B���N�g����ݒ�
	SetDllDirectory("Library/PhysX/bin/Debug");
	// DLL�̓ǂݍ���
	LoadLibraryExA("freeglutd.dll", NULL, NULL);
	LoadLibraryExA("PhysX_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCommon_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXCooking_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXDevice64.dll", NULL, NULL);
	LoadLibraryExA("PhysXFoundation_64.dll", NULL, NULL);
	LoadLibraryExA("PhysXGpu_64.dll", NULL, NULL);
	LoadLibraryExA("PVDRuntime_64.dll", NULL, NULL);
#endif // _DEBUG

}
