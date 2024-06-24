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

struct Transform
{
	Matrix World;		// ���[���h�s��
	Matrix View;		// �r���[�s��
	Matrix Proj;		// �ˉe�s��
};

struct LigBuffer
{
	Vector4 LigPos;		// ���C�g�̈ʒu
	Color LigColor;		// ���C�g�̐F
	Vector4 CameraPos;	// �J�����̈ʒu
};

struct MaterialBufer
{
	Vector3 BaseColor;	// �g�U���˗�
	float Alpha;		// ���ߓx
	float Metallic;		// �����x
	float Shininess;	// ���ʔ��ˋ��x
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

		if (!SearchFilePathW(L"Assets/Models/Cube/Cube.gltf", path))
		{
			ELOG("[App.cpp]Error : Line72 : �t�@�C����������܂���ł����B");
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

		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);

		// �}�e���A���̏�����
		if (!m_Material.Init(pPool, sizeof(MaterialBufer), resMaterial.size()))
		{
			ELOG("[App.cpp]Error : Line111 : �}�e���A���̏������Ɏ��s���܂����B");
			return false;
		}

		// �e�N�X�`���ƃ}�e���A���̐ݒ�
		for (size_t i = 0; i < resMaterial.size(); ++i)
		{
			auto ptr = m_Material.GetBufferPtr<MaterialBufer>(i);
			ptr->BaseColor = resMaterial[i].Diffuse;
			ptr->Alpha = resMaterial[i].Alpha;
			ptr->Metallic = 0.5f;
			ptr->Shininess = resMaterial[i].Shininess;

			// �e�N�X�`���̐ݒ�
			std::wstring path = dir + resMaterial[i].DiffuseMap;
			m_Material.SetTexture(i, TU_DIFFUSE, path);
		}

		// �g���I������̂ŉ��
		pPool->Release();
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
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		if (!pCB->Init(pPool, sizeof(LigBuffer)))
		{
			ELOG("[App.cpp]Error : Line144 : ���C�g�o�b�t�@�̏������Ɏ��s���܂����B");
			SAFE_DELETE(pCB);
			return false;
		}

		auto ptr = pCB->GetPtr<LigBuffer>();
		ptr->LigPos = Vector4(0.0f, 50.0f, 100.0f, 0.0f);
		ptr->LigColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
		ptr->CameraPos = Vector4(0.0f, 1.0f, 2.0f, 0.0f);

		m_pLight = pCB;

		// �g���I������̂ŉ��
		pPool->Release();
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
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = 0;

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
		param[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		// �X�^�e�B�b�N�T���v���[�̐ݒ�
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = -D3D12_FLOAT32_MAX;
		sampler.MaxLOD = +D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		// ���[�g�V�O�l�`���[�̐ݒ�
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = _countof(param);
		desc.NumStaticSamplers = 1;
		desc.pParameters = param;
		desc.pStaticSamplers = &sampler;
		desc.Flags = flag;

		// ���[�g�V�O�l�`���[�̐���
		ComPtr<ID3DBlob> pBlob;
		ComPtr<ID3DBlob> pError;

		// �V���A���C�Y
		auto hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, pBlob.GetAddressOf(), pError.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line257 : ���[�g�V�O�l�`���[�̃V���A���C�Y�Ɏ��s���܂����B");
			return false;
		}

		// ���[�g�V�O�l�`���[�̐���
		hr = RENDERER.GetDevice()->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(m_pRootSig.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line265 : ���[�g�V�O�l�`���[�̐����Ɏ��s���܂����B");
			return false;
		}
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		std::wstring vsPath;
		std::wstring psPath;

		// ���_�V�F�[�_������
		if (!SearchFilePathW(L"Assets/Shaders/VS_Phong.cso", vsPath))
		{
			ELOG("[App.cpp]Error : Line245 : ���_�V�F�[�_��������܂���ł����B");
			return false;
		}

		// �s�N�Z���V�F�[�_������
		if (!SearchFilePathW(L"Assets/Shaders/PS_Phong.cso", psPath))
		{
			ELOG("[App.cpp]Error : Line253 : �s�N�Z���V�F�[�_��������܂���ł����B");
			return false;
		}

		ComPtr<ID3DBlob> pVS;
		ComPtr<ID3DBlob> pPS;

		// ���_�V�F�[�_�̓ǂݍ���
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVS.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line272 : ���_�V�F�[�_�̓ǂݍ��݂Ɏ��s���܂����B");
			return false;
		}

		// �s�N�Z���V�F�[�_�̓ǂݍ���
		hr = D3DReadFileToBlob(psPath.c_str(), pPS.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line280 : �s�N�Z���V�F�[�_�̓ǂݍ��݂Ɏ��s���܂����B");
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

		// �p�C�v���C���X�e�[�g�̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = MeshVertex::InputLayout;
		desc.pRootSignature = m_pRootSig.Get();
		desc.VS = { pVS->GetBufferPointer(), pVS->GetBufferSize() };
		desc.PS = { pPS->GetBufferPointer(), pPS->GetBufferSize() };
		desc.RasterizerState = rsDesc;
		desc.BlendState = bsDesc;
		desc.DepthStencilState = dsDesc;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = RENDERER.GetColorTarget(0)->GetRTVDesc().Format;
		desc.DSVFormat = RENDERER.GetDepthTarget()->GetDSVDesc().Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// �p�C�v���C���X�e�[�g�̐���
		hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line345 : �p�C�v���C���X�e�[�g�̐����Ɏ��s���܂����B");
			return false;
		}
	}

	// �ϊ��s��p�̒萔�o�b�t�@�̐���
	{
		m_Transform.reserve(FRAME_BUFFER_COUNT);

		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{
			auto pCB = NEW ConstantBuffer();
			// �`�F�b�N
			if (!pCB)
			{
				ELOG("[App.cpp]Error : Line360 : �������̊m�ۂɎ��s���܂����B");
				return false;
			}

			// ������
			DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
			if (!pCB->Init(pPool, sizeof(Transform) * 2))
			{
				ELOG("[App.cpp]Error : Line376 : �ϊ��s��p�̒萔�o�b�t�@�̏������Ɏ��s���܂����B");
				SAFE_DELETE(pCB);
				return false;
			}

			// �J�����ݒ�.
			auto eyePos = Vector3(0.0f, 1.0f, 10.0f);
			auto targetPos = Vector3::Zero;
			auto upward = Vector3::UnitY;

			// ������p�ƃA�X�y�N�g��̐ݒ�.
			auto constexpr fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

			// �ϊ��s���ݒ�.
			auto ptr = pCB->GetPtr<Transform>();
			ptr->World = Matrix::Identity;
			ptr->View = Matrix::CreateLookAt(eyePos, targetPos, upward);
			ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 1.0f, 1000.0f);

			m_Transform.push_back(pCB);
		}
		m_RotateAngle = DirectX::XMConvertToRadians(-60.0f);
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

			auto frameIndex = RENDERER.GetFrameIndex();
			// teapot�̍X�V
			{
				m_RotateAngle += 0.025f;

				auto pTransform = m_Transform[frameIndex]->GetPtr<Transform>();
				pTransform->World = Matrix::CreateRotationY(m_RotateAngle);
			}

			// teapot�̕`��
			{
				auto pCmdList = RENDERER.GetCmdList()->Get();

				ID3D12DescriptorHeap* const pHeaps[] = { RENDERER.GetPool(POOL_TYPE_RES)->GetHeap() };

				// �p�C�v���C���X�e�[�g�̐ݒ�
				pCmdList->SetGraphicsRootSignature(m_pRootSig.Get());
				pCmdList->SetDescriptorHeaps(1, pHeaps);
				pCmdList->SetGraphicsRootConstantBufferView(0, m_Transform[frameIndex]->GetAddress());
				pCmdList->SetGraphicsRootConstantBufferView(1, m_pLight->GetAddress());
				pCmdList->SetPipelineState(m_pPSO.Get());
				auto viewport = RENDERER.GetViewport();
				auto scissor = RENDERER.GetScissor();
				pCmdList->RSSetViewports(1, &viewport);
				pCmdList->RSSetScissorRects(1, &scissor);

				// ���b�V���̕`��
				for (size_t i = 0; i < m_pMesh.size(); ++i)
				{
					// �}�e���A���̐ݒ�
					auto id = m_pMesh[i]->GetMaterialId();

					// �萔�o�b�t�@��ݒ�
					pCmdList->SetGraphicsRootConstantBufferView(2, m_Material.GetBufferAddress(i));

					// �e�N�X�`����ݒ�
					auto pTexture = m_Material.GetTextureHandle(id, TU_DIFFUSE);
					pCmdList->SetGraphicsRootDescriptorTable(3, pTexture);

					// �`��
					m_pMesh[i]->Draw();
				}

			}

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

	// ���C�g�o�b�t�@�̉��
	SAFE_DELETE(m_pLight);

	// �ϊ��s��p�̒萔�o�b�t�@�̊J��
	for (auto& cb : m_Transform)
	{
		SAFE_UNINIT(cb);
	}
	m_Transform.clear();
	m_Transform.shrink_to_fit();

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
