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

struct alignas(256) CbMesh
{
	Matrix   World;      // ���[���h�s��
};

struct alignas(256) CbTransform
{
	Matrix   View;       // �r���[�s��
	Matrix   Proj;       // �ˉe�s��
};

struct alignas(256) CbLight
{
	Vector3  LightColor;        // ���C�g�J���[
	float    LightIntensity;    // ���C�g���x
	Vector3  LightForward;      // ���C�g�̏Ǝ˕���
};

struct alignas(256) CbCamera
{
	Vector3  CameraPosition;    // �J�����ʒu�ł�
};

struct alignas(256) CbMaterial
{
	Vector3 BaseColor;  // ��{�F
	float   Alpha;      // ���ߓx
	float   Roughness;  // �ʂ̑e��(�͈͂�0~1)
	float   Metallic;   // �����x(�͈͂�[0~1])
};

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

	// SceneManager�̏�����
	SCENE_MANAGER.Init();

	// ShaderManager�̏�����
	SHADER_MANAGER.Init();

	// ���b�V�������[�h
	{
		std::wstring path;
		// �t�@�C���p�X������

		if (!SearchFilePathW(L"Assets/Models/material/material_test.obj", path))
		{
			ELOG("[App.cpp]Error : Line81 : �t�@�C����������܂���ł����B");
			return false;
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// ���b�V���̓ǂݍ���
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line93 : ���b�V���̓ǂݍ��݂Ɏ��s���܂����BFilePath = %ls", path.c_str());
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
				ELOG("[App.cpp]Error : Line109 : �������̊m�ۂɎ��s���܂����B");
				return false;
			}

			// ����������
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line116 : ���b�V���̏������Ɏ��s���܂����B");
				SAFE_DELETE(pMesh);
				return false;
			}

			// ���b�V���̒ǉ�
			m_pMesh.push_back(pMesh);
		}

		// �������œK��
		resMesh.shrink_to_fit();

		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);

		// �}�e���A���̏�����
		if (!m_Material.Init(pPool, sizeof(CbMaterial), resMaterial.size()))
		{
			ELOG("[App.cpp]Error : Line133 : �}�e���A���̏������Ɏ��s���܂����B");
			return false;
		}

		// �e�N�X�`���ƃ}�e���A����ݒ�.
		{
			/* �����ł̓}�e���A�������ߑł��ł��邱�Ƃ�O��Ƀn�[�h�R�[�f�B���O���Ă��܂�. */
			m_Material.SetTexture(0, TU_BASE_COLOR, dir + L"wall_bc.dds");
			m_Material.SetTexture(0, TU_METALLIC, dir + L"wall_m.dds");
			m_Material.SetTexture(0, TU_ROUGHNESS, dir + L"wall_r.dds");
			m_Material.SetTexture(0, TU_NORMAL, dir + L"wall_n.dds");

			m_Material.SetTexture(1, TU_BASE_COLOR, dir + L"matball_bc.dds");
			m_Material.SetTexture(1, TU_METALLIC, dir + L"matball_m.dds");
			m_Material.SetTexture(1, TU_ROUGHNESS, dir + L"matball_r.dds");
			m_Material.SetTexture(1, TU_NORMAL, dir + L"matball_n.dds");
		}
	}

	// ���C�g�o�b�t�@�̐ݒ�
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_LightCB[i].Init(pPool, sizeof(CbLight)))
			{
				ELOG("Error : Line159 : ConstantBuffer::Init() Failed.");
				return false;
			}
		}
	}

	// �J�����o�b�t�@�̐ݒ�.
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_CameraCB[i].Init(pPool, sizeof(CbCamera)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}
		}
	}

	// ���[�g�V�O�l�`���[�̐���
	{
		RootSignature::Desc desc;
		desc.Begin(8)
			.SetCBV(ShaderStage::VS, 0, 0)
			.SetCBV(ShaderStage::VS, 1, 1)
			.SetCBV(ShaderStage::PS, 2, 1)
			.SetCBV(ShaderStage::PS, 3, 2)
			.SetSRV(ShaderStage::PS, 4, 0)
			.SetSRV(ShaderStage::PS, 5, 1)
			.SetSRV(ShaderStage::PS, 6, 2)
			.SetSRV(ShaderStage::PS, 7, 3)
			.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
			.AddStaticSmp(ShaderStage::PS, 1, SamplerState::LinearWrap)
			.AddStaticSmp(ShaderStage::PS, 2, SamplerState::LinearWrap)
			.AddStaticSmp(ShaderStage::PS, 3, SamplerState::LinearWrap)
			.AllowIL()
			.End();

		if (!m_SceneRootSig.Init(desc.GetDesc()))
		{
			ELOG("Error : RootSignature::Init() Failed.");
			return false;
		}
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		// ���_�V�F�[�_�̓ǂݍ���
		if (!SHADER_MANAGER.LoadShader("Assets/Shaders/VS_Basic.cso", "VS_Basic", ShaderManager::ShaderType::Vertex))
		{
			ELOG("[App.cpp]Error : Line222 : ���_�V�F�[�_��������܂���ł����B");
			return false;
		}

		// �s�N�Z���V�F�[�_�̓ǂݍ���
		if (!SHADER_MANAGER.LoadShader("Assets/Shaders/PS_Basic.cso", "PS_Basic", ShaderManager::ShaderType::Pixel))
		{
			ELOG("[App.cpp]Error : Line229 : �s�N�Z���V�F�[�_��������܂���ł����B");
			return false;
		}

		// ���X�^���C�U�X�e�[�g�̐ݒ�
		D3D12_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D12_FILL_MODE_SOLID;								// ���_�ɂ���Č`����Ă���O�p�`�ŕ`��
		rsDesc.CullMode = D3D12_CULL_MODE_BACK;									// �w�ʃJ�����O
		rsDesc.FrontCounterClockwise = TRUE;									// �����v��肪�\��
		rsDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;							// �g�p���Ȃ��̂Ńf�t�H���g
		rsDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;					// �g�p���Ȃ��̂Ńf�t�H���g
		rsDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;	// �g�p���Ȃ��̂Ńf�t�H���g
		rsDesc.DepthClipEnable = FALSE;											// �g�p���Ȃ��̂�FALSE
		rsDesc.MultisampleEnable = FALSE;										// �g�p���Ȃ��̂�FALSE
		rsDesc.AntialiasedLineEnable = FALSE;									// �g�p���Ȃ��̂�FALSE
		rsDesc.ForcedSampleCount = 0;											// 0 : �T���v��������������Ȃ�
		rsDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF : �ێ�I�ȃ��X�^���C�[�[�V�����𖳌��ɂ���

		// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�(a�u�����h)
		D3D12_RENDER_TARGET_BLEND_DESC rtDesc =
		{
			TRUE,FALSE,
			D3D12_BLEND_SRC_ALPHA,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		// �u�����h�X�e�[�g�̐ݒ�
		D3D12_BLEND_DESC bsDesc = {};
		bsDesc.AlphaToCoverageEnable = FALSE;
		bsDesc.IndependentBlendEnable = FALSE;
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			bsDesc.RenderTarget[i] = rtDesc;
		}

		// �[�x�X�e���V���X�e�[�g�̐ݒ�
		D3D12_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;

		// ���̓��C�A�E�g�̐ݒ�
		D3D12_INPUT_ELEMENT_DESC elements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// �p�C�v���C���X�e�[�g�̐ݒ�
		auto pVS = SHADER_MANAGER.GetShader("VS_Basic");
		auto pPS = SHADER_MANAGER.GetShader("PS_Basic");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { elements ,4 };
		desc.pRootSignature = m_SceneRootSig.GetPtr();
		desc.VS = { pVS->GetBufferPointer(), pVS->GetBufferSize() };
		desc.PS = { pPS->GetBufferPointer(), pPS->GetBufferSize() };
		desc.RasterizerState = rsDesc;
		desc.BlendState = bsDesc;
		desc.DepthStencilState = dsDesc;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = RENDERER.GetSceneColorTarget()->GetRTVDesc().Format;
		desc.DSVFormat = RENDERER.GetSceneDepthTarget()->GetDSVDesc().Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// �p�C�v���C���X�e�[�g�̐���
		hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pScenePSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line336 : �p�C�v���C���X�e�[�g�̐����Ɏ��s���܂����B");
			return false;
		}
	}

	// �ϊ��s��p�̒萔�o�b�t�@�̐���.
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{

			// �萔�o�b�t�@������.
			if (!m_TransformCB[i].Init(pPool, sizeof(CbTransform)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			// �J�����ݒ�.
			auto eyePos = Vector3(0.0f, 1.0f, 2.0f);
			auto targetPos = Vector3::Zero;
			auto upward = Vector3::UnitY;

			// ������p�ƃA�X�y�N�g��̐ݒ�.
			constexpr auto fovY = DirectX::XMConvertToRadians(90.0f);
			auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

			// �ϊ��s���ݒ�.
			auto ptr = m_TransformCB[i].GetPtr<CbTransform>();
			ptr->View = Matrix::CreateLookAt(eyePos, targetPos, upward);
			ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
		}

		m_RotateAngle = DirectX::XMConvertToRadians(-60.0f);
	}

	// ���b�V���p�o�b�t�@�̐���.
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_MeshCB[i].Init(pPool, sizeof(CbMesh)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			auto ptr = m_MeshCB[i].GetPtr<CbMesh>();
			ptr->World = Matrix::Identity;
		}
	}

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
			DrawScene();

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

	// ���b�V���̉��
	for (auto& mesh : m_pMesh)
	{
		SAFE_UNINIT(mesh);
	}
	m_pMesh.clear();
	m_pMesh.shrink_to_fit();

	// �}�e���A���̉��
	m_Material.Uninit();

	// �萔�o�b�t�@�̉��
	for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_LightCB[i].Uninit();
		m_CameraCB[i].Uninit();
		m_TransformCB[i].Uninit();
		m_MeshCB[i].Uninit();
	}

	// ���[�g�V�O�l�`���̉��
	m_SceneRootSig.Uninit();

	// �p�C�v���C���X�e�[�g�̉��
	m_pScenePSO.Reset();

	// �}�l�[�W���[�̉��
	SHADER_MANAGER.Uninit();
	SCENE_MANAGER.Uninit();
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

// �V�[���̕`��
void Application::DrawScene()
{
	auto pCmd = RENDERER.GetCmdList()->Get();
	auto cameraPos = Vector3(2.0f, 3.0f, 4.0f);
	auto frameIndex = RENDERER.GetFrameIndex();

	// ���C�g�o�b�t�@�̍X�V
	{
		Matrix matrix = Matrix::CreateRotationY(m_RotateAngle);

		auto ptr = m_LightCB[frameIndex].GetPtr<CbLight>();
		ptr->LightColor = Vector3(1.0f, 1.0f, 1.0f);
		ptr->LightForward = Vector3::TransformNormal(Vector3(0.0f, 1.0f, 1.0f), matrix);
		ptr->LightIntensity = 2.0f;
		m_RotateAngle += 0.01f;
	}
	// �J�����o�b�t�@�̍X�V.
	{
		auto ptr = m_CameraCB[frameIndex].GetPtr<CbCamera>();
		ptr->CameraPosition = cameraPos;
	}
	// ���b�V���̃��[���h�s��̍X�V
	{
		auto ptr = m_MeshCB[frameIndex].GetPtr<CbMesh>();
		ptr->World = Matrix::Identity;
	}
	// �ϊ��p�����[�^�̍X�V
	{
		constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
		auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

		auto ptr = m_TransformCB[frameIndex].GetPtr<CbTransform>();
		ptr->View = Matrix::CreateLookAt(cameraPos, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
	}

	pCmd->SetGraphicsRootSignature(m_SceneRootSig.GetPtr());
	pCmd->SetPipelineState(m_pScenePSO.Get());
	pCmd->SetGraphicsRootDescriptorTable(0, m_TransformCB[frameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(2, m_LightCB[frameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(3, m_CameraCB[frameIndex].GetHandleGPU());

	// �I�u�W�F�N�g��`��.
	{
		pCmd->SetGraphicsRootDescriptorTable(1, m_MeshCB[frameIndex].GetHandleGPU());
		DrawMesh();
	}
}


void Application::DrawMesh()
{
	auto pCmd = RENDERER.GetCmdList()->Get();
	for (size_t i = 0; i < m_pMesh.size(); ++i)
	{
		// �}�e���A��ID���擾.
		auto id = m_pMesh[i]->GetMaterialId();

		// �e�N�X�`����ݒ�.
		pCmd->SetGraphicsRootDescriptorTable(4, m_Material.GetTextureHandle(id, TU_BASE_COLOR));
		pCmd->SetGraphicsRootDescriptorTable(5, m_Material.GetTextureHandle(id, TU_METALLIC));
		pCmd->SetGraphicsRootDescriptorTable(6, m_Material.GetTextureHandle(id, TU_ROUGHNESS));
		pCmd->SetGraphicsRootDescriptorTable(7, m_Material.GetTextureHandle(id, TU_NORMAL));

		// ���b�V����`��.
		m_pMesh[i]->Draw();
	}
}
