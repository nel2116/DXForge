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

using namespace DirectX::SimpleMath;

struct LigBuffer
{
	Vector4 LigPos;	// ���C�g�̈ʒu
	Color LigColor;	// ���C�g�̐F
};

struct MaterialBufer
{
	Vector3 Diffuse;	// �f�B�t���[�Y
	float Alpha;		// �A���t�@
};

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

	// ���b�V�������[�h
	{
		std::wstring path;
		// �t�@�C���p�X������
		if (!SearchFilePathW(L"Assets/Models/teapot/teapot.obj", path));
		{
			ELOG("[App.cpp]Error : Line61 : �t�@�C����������܂���ł����B");
			return false;
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// ���b�V���̓ǂݍ���
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line72 : ���b�V���̓ǂݍ��݂Ɏ��s���܂����BFilePath = %ls", path.c_str());
			return false;
		}

		// ��������\��
		m_pMesh.reserve(resMesh.size());

		// ���b�V����������
		for (size_t i = 0; i < resMesh.size(); ++i)
		{
			// ���b�V���̐���
			auto pMesh = NEW Mesh();

			// �`�F�b�N
			if (!pMesh)
			{
				ELOG("[App.cpp]Error : Line88 : �������̊m�ۂɎ��s���܂����B");
				return false;
			}

			// ����������
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line96 : ���b�V���̏������Ɏ��s���܂����B");
				SAFE_DELETE(pMesh);
				return false;
			}

			// ���b�V���̒ǉ�
			m_pMesh.push_back(pMesh);
		}

		// �������œK��
		resMesh.shrink_to_fit();

		DescriptorPool* pPool = RENDERER.GetDescriptorPool(POOL_TYPE_RES);

		// �}�e���A���̏�����
		if (!m_Material.Init(pPool, sizeof(MaterialBufer), resMaterial.size()));
		{
			ELOG("[App.cpp]Error : Line111 : �}�e���A���̏������Ɏ��s���܂����B");
			return false;
		}

		// �e�N�X�`���ƃ}�e���A���̐ݒ�
		for (size_t i = 0; i < resMaterial.size(); ++i)
		{
			auto ptr = m_Material.GetBufferPtr<MaterialBufer>(i);
			ptr->Diffuse = resMaterial[i].Diffuse;
			ptr->Alpha = resMaterial[i].Alpha;

			// �e�N�X�`���̐ݒ�
			std::wstring path = dir + resMaterial[i].DiffuseMap;
			m_Material.SetTexture(i, TU_DIFFUSE, path);
		}
	}

	// ���C�g�o�b�t�@�̐ݒ�
	{
		auto pCB = NEW ConstantBuffer();
		// �`�F�b�N
		if (!pCB)
		{
			ELOG("[App.cpp]Error : Line136 : �������̊m�ۂɎ��s���܂����B");
			return false;
		}

		// ������
		DescriptorPool* pPool = RENDERER.GetDescriptorPool(POOL_TYPE_RES);
		if (!pCB->Init(pPool, sizeof(LigBuffer)))
		{
			ELOG("[App.cpp]Error : Line144 : ���C�g�o�b�t�@�̏������Ɏ��s���܂����B");
			SAFE_DELETE(pCB);
			return false;
		}

		auto ptr = pCB->GetPtr<LigBuffer>();
		ptr->LigPos = Vector4(0.0f, 50.0f, 100.0f, 1.0f);
		ptr->LigColor = Color(1.0f, 1.0f, 1.0f, 0.0f);

		m_pLight = pCB;
	}

	// ���[�g�V�O�l�`���[�̐���
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		// ���[�g�p�����[�^�[�̐ݒ�
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// �f�B�X�N���v�^�����W��ݒ�
		D3D12_DESCRIPTOR_RANGE range = {};
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].NumDescriptors = 1;
		range[0].BaseShaderRegister = 0;
		range[0].RegisterSpace = 0;
		range[0].OffsetInDescriptorsFromTableStart = 0;

		// ���[�g�p�����[�^�[��ݒ�
		D3D12_ROOT_PARAMETER param[4] = {};
		param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[0].Descriptor.ShaderRegister = 0;
		param[0].Descriptor.RegisterSpace = 0;
		param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[1].Descriptor.ShaderRegister = 1;
		param[1].Descriptor.RegisterSpace = 0;
		param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		param[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[2].Descriptor.ShaderRegister = 2;
		param[2].Descriptor.RegisterSpace = 0;
		param[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		param[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param[3].DescriptorTable.NumDescriptorRanges = 1;
		param[3].DescriptorTable.pDescriptorRanges = &range;

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
