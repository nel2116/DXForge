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

	if (!Renderer::Instance().Init(&m_window))
	{
		assert(0 && "Renderer�̏������Ɏ��s���܂����B");
		return false;
	}

	if (!Texture::Init())
	{
		assert(0 && "Texture�̏������Ɏ��s���܂����B");
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
	Model model;
	model.Load("Assets/Model/Cube/Cube.gltf");

	RenderingSetting setting = {};
	setting.inputLayouts = { InputLayout::POSITION,InputLayout::TEXCOORD,InputLayout::COLOR,InputLayout::NORMAL,InputLayout::TANGENT };
	setting.Formats = { DXGI_FORMAT_R8G8B8A8_UNORM };
	setting.cullMode = CullMode::Back;
	setting.primitiveTopology = PrimitiveTopology::Triangle;

	Shader shader;
	string path = "Simple";
	shader.Create(&Renderer::Instance(), path, setting, { RangeType::CBV,RangeType::CBV,RangeType::SRV,RangeType::SRV,RangeType::SRV,RangeType::SRV });

	DirectX::XMMATRIX mWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mWorld2 = DirectX::XMMatrixIdentity();
	// �J�����̐ݒ�
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), ASPECT_RATIO, 0.01, 1000.0f);

	// 2D�p�J�����̐ݒ�
	DirectX::XMMATRIX mView2 = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX mProj2 = DirectX::XMMatrixOrthographicLH(WINDOW_WIDTH, WINDOW_HEIGHT, 0.01, 1000.0f);

	CBufferData::Camera cbCamera;
	CBufferData::Camera cbCamera2;
	cbCamera.mView = mView;
	cbCamera.mProj = mProj;
	cbCamera2.mView = mView2;
	cbCamera2.mProj = mProj2;

	// �e�N�X�`���̐ݒ�
	setting.inputLayouts = { InputLayout::POSITION };
	setting.isDepth = false;
	setting.isDepthMask = false;

	Shader texShader;
	path = "Texture";
	texShader.Create(&Renderer::Instance(), path, setting, { RangeType::CBV,RangeType::CBV,RangeType::SRV });

	Texture tex;
	tex.Load(&Renderer::Instance(), "Assets/Texture/field.jpg");

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
			Renderer::Instance().Begin3DDraw();		// �`��J�n
			// �����ɕ`�揈��������
			Renderer::Instance().GetCBVSRVUAVHeap()->SetHeap();
			Renderer::Instance().GetCBufferAllocater()->ResetNumber();

			// �V�F�[�_�[�̐ݒ�
			shader.Begin(m_window.GetWidth(), m_window.GetHeight());
			// �V�F�[�_�[�ɃJ�����̃f�[�^��n��
			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(0, cbCamera);
			// ���[���h�s��̍X�V
			mWorld *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(1.0f));
			mWorld *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(1.0f));
			mWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(1.0f));
			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(1, mWorld);
			// ���f���̕`��
			shader.DrawModel(model);

			// �e�N�X�`���̕`��
			Renderer::Instance().Begin2DDraw();		// �`��J�n

			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(0, cbCamera2);
			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(1, mWorld2);
			texShader.Begin(m_window.GetWidth(), m_window.GetHeight());
			texShader.Draw2D(tex);

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
