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
#include <Graphics/Mesh/Mesh.h>
#include <Graphics/Shader/Shader.h>

// ====== �����o�֐� ======
bool Application::Init()
{
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

	if (!Renderer::Instance().Init(&m_window))
	{
		assert(0 && "Renderer�̏������Ɏ��s���܂����B");
		return false;
	}

	// �^�C�}�[�̏�����
	timeBeginPeriod(1);
	m_dwExecLastTime = timeGetTime();
	m_dwCurrentTime = m_dwExecLastTime;
	return true;
}

void Application::Run()
{
	Mesh mesh;
	mesh.Create(&Renderer::Instance());

	RenderingSetting setting = {};
	setting.inputLayouts = { InputLayout::POSITION };
	setting.Formats = { DXGI_FORMAT_R8G8B8A8_UNORM };
	setting.isDepth = false;
	setting.isDepthMask = false;
	setting.cullMode = CullMode::None;

	Shader shader;
	string path = "Simple";
	shader.Create(&Renderer::Instance(), path, setting, {});

	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }
		m_dwCurrentTime = timeGetTime();
		if ((m_dwCurrentTime - m_dwExecLastTime) >= FRAME_TIME)
		{
			m_dwExecLastTime = m_dwCurrentTime;
			// ====== �����ɃQ�[���̏��������� ======
			// ------ �X�V���� ------

			// ------ �`�揈�� ------
			Renderer::Instance().BeginDraw();		// �`��J�n
			// �����ɕ`�揈��������

			shader.Begin(m_window.GetWidth(), m_window.GetHeight());

			shader.DrawMesh(mesh);


			Renderer::Instance().EndDraw();			// �`��I��
		}
	}
}

void Application::Uninit()
{
	Renderer::Instance().Uninit();	// Renderer�̏I������
	timeEndPeriod(1);	// �^�C�}�[�̏I������
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// �E�B���h�E�N���X�̓o�^����
	CoUninitialize();	// COM�̏I������
}
