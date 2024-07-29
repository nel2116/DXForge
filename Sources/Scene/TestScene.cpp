#include "stdafx.h"
#include "TestScene.h"
#include <System/Input.h>
#include <Actor/TestActor.h>
#include <Component/RigidBodyComponent.h>
#include <Component/ModelComponent.h>

struct alignas(256) CbLight
{
	DirectX::SimpleMath::Vector3  LightColor;   // ���C�g�J���[
	float    LightIntensity;					// ���C�g���x
	DirectX::SimpleMath::Vector3  LightForward; // ���C�g�̏Ǝ˕���
};

struct alignas(256) CbTransform
{
	DirectX::SimpleMath::Matrix   View;			// �r���[�s��
	DirectX::SimpleMath::Matrix   Proj;			// �ˉe�s��
};

struct alignas(256) CbCamera
{
	DirectX::SimpleMath::Vector3  CameraPosition;    // �J�����ʒu�ł�
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
		ptr->CameraPosition = m_pCamera->GetPosition();
	}

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
	pCmd->SetGraphicsRootDescriptorTable(3, m_CameraCB[frameIndex].GetHandleGPU());
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
	model->LoadModel("Assets/Models/material/material_test.obj");

	// ���C�g�o�b�t�@�̐ݒ�
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_LightCB[i].Init(pPool, sizeof(CbLight)))
			{
				ELOG("Error : Line159 : ConstantBuffer::Init() Failed.");
			}
		}
	}

	// �J�����o�b�t�@�̐ݒ�.
	{
		m_pCamera = NEW Camera();
		m_pCamera->SetPosition(Vector3(0.2f, 0.3f, 0.4f));

		m_pProjector = NEW Projector();
		m_pProjector->SetPerspective(DirectX::XMConvertToRadians(90.0f), static_cast<float>(1280) / 720, 0.1f, 1000.0f);

		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_CameraCB[i].Init(pPool, sizeof(CbCamera)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
			}
		}
	}

	// ���[�g�V�O�l�`���[�̐���
	{
		m_pSceneRootSig = NEW RootSignature();
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
		m_CameraCB[i].Uninit();
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
