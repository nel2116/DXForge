#include "stdafx.h"
#include "Application.h"
#include <System/Window.h>
#include <System/Dx12Wrapper.h>
#include <System/PMDRenderer.h>
#include <Actor/PMDActor.h>

Application& Application::Instance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		return false;
	}
	CreateMainWindow(m_hwnd, m_windowClass);

	// Dx12�̏�����
	m_dx12 = NEW Dx12Wrapper(m_hwnd);
	m_pmdRenderer = NEW PMDRenderer(*m_dx12);
	m_pmdActor = NEW PMDActor("Assets/Model/�����~�N.pmd", *m_pmdRenderer);

	return true;
}

void Application::Run()
{
	ShowWindow(m_hwnd, SW_SHOW);	// �E�B���h�E��\������
	float angle = 0.0f;
	MSG msg = {};
	unsigned int frame = 0;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	// �L�[���̓��b�Z�[�W����������
			DispatchMessage(&msg);	// ���b�Z�[�W����������
		}
		// ����Application���I�����ĂƂ��Ƀ��b�Z�[�W��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT) { break; }

		// �S�̂̕`�揀��
		m_dx12->BeginDraw();

		// PMD�`��̃p�C�v���C���ɍ��킹��
		m_dx12->GetCommandList()->SetPipelineState(m_pmdRenderer->GetPipelineState());
		// ���[�g�V�O�l�`���[���ݒ�
		m_dx12->GetCommandList()->SetGraphicsRootSignature(m_pmdRenderer->GetRootSignature());

		m_dx12->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_dx12->SetScene();

		// ���f���̕`��
		m_pmdActor->Update();
		m_pmdActor->Draw();

		// �S�̂̕`��I��
		m_dx12->EndDraw();

		// �t���b�v
		m_dx12->GetSwapChain()->Present(1, 0);
	}
}

void Application::Terminate()
{
	// �����N���X�g��񂩂�o�^�������Ă�
	UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);

	// �������
	SAFE_DELETE(m_pmdActor);		// �A�N�^�[�̉��
	SAFE_DELETE(m_pmdRenderer);	// �����_���[�̉��

	CoUninitialize();	// COM�̏I������

	ID3D12DebugDevice* pDebugDevice = nullptr;
	m_dx12->GetDevice()->QueryInterface(&pDebugDevice);
	SAFE_DELETE(m_dx12);	// Dx12�̉��
	pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	pDebugDevice->Release();
}

SIZE Application::GetWindowSize() const
{
	SIZE ret;
	ret.cx = WINDOW_WIDTH;
	ret.cy = WINDOW_HEIGHT;
	return ret;
}

Application::Application()
{
}

Application::~Application()
{
}
