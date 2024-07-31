#include "stdafx.h"
#include "TestScene.h"
#include <System/Input.h>
#include <Actor/TestActor.h>
#include <Component/RigidBodyComponent.h>
#include <Component/ModelComponent.h>

struct alignas(256) CbLight
{
	Vector4  LightPosition;     // ���C�g�ʒu
	Color    LightColor;        // ���C�g�J���[
	Vector4  CameraPosition;    // �J�����ʒu
};

struct alignas(256) CbTransform
{
	Matrix   View;			// �r���[�s��
	Matrix   Proj;			// �ˉe�s��
};

void TestScene::Update()
{
	auto body = ACTOR_MANAGER.GetActor<TestActor>()->GetComponent<RigidBodyComponent>();

	if (IsKeyPress('S'))
	{
		body->SetVelocity(Vector3(0.0f, 0.0f, 1.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('W'))
	{
		body->SetVelocity(Vector3(0.0f, 0.0f, -1.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('A'))
	{
		body->SetVelocity(Vector3(-1.0f, 0.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('D'))
	{
		body->SetVelocity(Vector3(1.0f, 0.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('Q'))
	{
		body->SetVelocity(Vector3(0.0f, 1.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('E'))
	{
		body->SetVelocity(Vector3(0.0f, -1.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}

}

void TestScene::Draw()
{
	auto pCmd = RENDERER.GetCmdList()->Get();
	auto frameIndex = RENDERER.GetFrameIndex();

	// �ϊ��p�����[�^�̍X�V
	{
		auto ptr = m_TransformCB[frameIndex].GetPtr<CbTransform>();
		ptr->View = m_pCamera->GetView();
		ptr->Proj = m_pProjector->GetMatrix();
	}

	pCmd->SetGraphicsRootSignature(m_pSceneRootSig->GetPtr());
	pCmd->SetPipelineState(m_pScenePSO->GetPtr());
	pCmd->SetGraphicsRootDescriptorTable(0, m_TransformCB[frameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(2, m_LightCB[frameIndex].GetHandleGPU());
}

void TestScene::Init()
{
	// PhysX�̏�����
	auto pPhysics = SCENE_MANAGER.GetPhysics();

	// ���ʂ�ǉ�
	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(
		*pPhysics, physx::PxPlane(0, 1, 0, 0),
		*pPhysics->createMaterial(0.5f, 0.5f, 0.5f)
	);

	// �V�[���ɒǉ�
	if (groundPlane)
	{
		// �V�[���ɒǉ�
		m_pScene->addActor(*groundPlane);
	}
	else
	{
		ELOG("Error : Plane creation failed.");
	}

	// �e�X�g�A�N�^�[��ǉ�
	TestActor* pActor = ACTOR_MANAGER.AddActor<TestActor>();
	auto body = pActor->AddComponent<RigidBodyComponent>();
	body->SetRigidBody(RigidBodyComponent::ActorType::Dynamic, RigidBodyComponent::ShapeType::Box);

	auto model = pActor->AddComponent<ModelComponent>();
	model->LoadModel("Assets/Models/teapot/teapot.obj");

	// �J�����o�b�t�@�̐ݒ�.
	{
		m_pCamera = NEW Camera();
		m_pCamera->SetPosition(Vector3(0.0f, 10.0f, 20.0f));

		m_pProjector = NEW Projector();
		m_pProjector->SetPerspective(DirectX::XMConvertToRadians(90.0f), static_cast<float>(1280) / 720, 0.1f, 1000.0f);
	}

	// ���C�g�o�b�t�@�̐ݒ�
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_LightCB[i].Init(pPool, sizeof(CbLight)))
			{
				ELOG("Error : Line159 : ConstantBuffer::Init() Failed.");
			}

			auto ptr = m_LightCB[i].GetPtr<CbLight>();
			ptr->LightPosition = Vector4(0.0f, 50.0f, 100.0f, 0.0f);
			ptr->LightColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
			Vector3 campos = m_pCamera->GetPosition();
			ptr->CameraPosition = Vector4(campos.x, campos.y, campos.z, 0.0f);
		}
	}

	// ���[�g�V�O�l�`���[�̐���
	{
		m_pSceneRootSig = NEW RootSignature();
		RootSignature::Desc desc;
		desc.Begin(5)
			.SetCBV(ShaderStage::VS, 0, 0) // CbTransform
			.SetCBV(ShaderStage::VS, 1, 1) // CbMesh
			.SetCBV(ShaderStage::PS, 2, 2) // LightBuffer
			.SetCBV(ShaderStage::PS, 3, 3) // MaterialBuffer
			.SetSRV(ShaderStage::PS, 4, 0) // Texture
			.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
			.AllowIL()
			.End();

		if (!m_pSceneRootSig->Init(desc.GetDesc()))
		{
			ELOG("Error : RootSignature::Init() Failed.");
		}
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		m_pScenePSO = NEW PipelineState();

		// ���_�V�F�[�_�̓ǂݍ���
		if (!SHADER_MANAGER.LoadShader("Assets/Shaders/VS_Basic.cso", "VS_Basic", ShaderManager::ShaderType::Vertex))
		{
			ELOG("[App.cpp]Error : Line222 : ���_�V�F�[�_��������܂���ł����B");
		}

		// �s�N�Z���V�F�[�_�̓ǂݍ���
		if (!SHADER_MANAGER.LoadShader("Assets/Shaders/PS_Basic.cso", "PS_Basic", ShaderManager::ShaderType::Pixel))
		{
			ELOG("[App.cpp]Error : Line229 : �s�N�Z���V�F�[�_��������܂���ł����B");
		}

		auto pVS = SHADER_MANAGER.GetShader("VS_Basic");
		auto pPS = SHADER_MANAGER.GetShader("PS_Basic");

		// �p�C�v���C���X�e�[�g�̐ݒ�
		PipelineState::Desc desc;
		desc.SetVertexShader({ pVS->GetBufferPointer(), pVS->GetBufferSize() })
			.SetPixelShader({ pPS->GetBufferPointer(), pPS->GetBufferSize() })
			.SetRootSignature(m_pSceneRootSig->GetPtr())
			.SetRenderTargetFormat(RENDERER.GetSceneColorTarget()->GetRTVDesc().Format, RENDERER.GetSceneDepthTarget()->GetDSVDesc().Format);

		// �J�X�^���ݒ肪�K�v�ȏꍇ�̂ݐݒ���㏑��
		// ���X�^���C�U�[�X�e�[�g�̐ݒ�
		D3D12_RASTERIZER_DESC rsDesc = desc.GetDesc()->RasterizerState;
		rsDesc.FrontCounterClockwise = TRUE; // �J�X�^���ݒ�
		rsDesc.DepthClipEnable = FALSE; // �J�X�^���ݒ�
		desc.SetRasterizerState(rsDesc);

		// ���̓��C�A�E�g�̐ݒ�
		D3D12_INPUT_ELEMENT_DESC elements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { elements, _countof(elements) };
		desc.SetInputLayout(inputLayoutDesc);

		// �p�C�v���C���X�e�[�g�̐ݒ�
		if (!m_pScenePSO->Init(desc.GetDesc()))
		{
			ELOG("Error : PipelineState::Init() Failed.");
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
			}

			// �ϊ��s���ݒ�.
			auto ptr = m_TransformCB[i].GetPtr<CbTransform>();
			ptr->View = m_pCamera->GetView();
			ptr->Proj = m_pProjector->GetMatrix();
		}

		m_RotateAngle = DirectX::XMConvertToRadians(-60.0f);
	}

}

void TestScene::Uninit()
{
	// �J�����̉��
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pProjector);

	// �萔�o�b�t�@�̉��
	for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_LightCB[i].Uninit();
		m_TransformCB[i].Uninit();
	}

	// ���[�g�V�O�l�`���̉��
	m_pSceneRootSig->Uninit();
	SAFE_DELETE(m_pSceneRootSig);

	// �p�C�v���C���X�e�[�g�̉��
	m_pScenePSO->Uninit();
	SAFE_DELETE(m_pScenePSO);

	// �V�[���̉��
	if (m_pScene)
	{
		m_pScene->release();
		m_pScene = nullptr;
	}
}
