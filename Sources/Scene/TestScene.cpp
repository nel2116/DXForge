#include "stdafx.h"
#include "TestScene.h"
#include <System/Input.h>
#include <Actor/TestActor.h>
#include <Component/RigidBodyComponent.h>
#include <Component/ModelComponent.h>

struct alignas(256) CbLight
{
	DirectX::SimpleMath::Vector3  LightColor;   // ライトカラー
	float    LightIntensity;					// ライト強度
	DirectX::SimpleMath::Vector3  LightForward; // ライトの照射方向
};

struct alignas(256) CbTransform
{
	DirectX::SimpleMath::Matrix   View;			// ビュー行列
	DirectX::SimpleMath::Matrix   Proj;			// 射影行列
};

struct alignas(256) CbCamera
{
	DirectX::SimpleMath::Vector3  CameraPosition;    // カメラ位置です
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

	// ライトバッファの更新
	{
		Matrix matrix = Matrix::CreateRotationY(m_RotateAngle);

		auto ptr = m_LightCB[frameIndex].GetPtr<CbLight>();
		ptr->LightColor = Vector3(1.0f, 1.0f, 1.0f);
		ptr->LightForward = Vector3::TransformNormal(Vector3(0.0f, 1.0f, 1.0f), matrix);
		ptr->LightIntensity = 2.0f;
		m_RotateAngle += 0.01f;
	}

	// カメラバッファの更新.
	{
		auto ptr = m_CameraCB[frameIndex].GetPtr<CbCamera>();
		ptr->CameraPosition = m_pCamera->GetPosition();
	}

	// 変換パラメータの更新
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
	// PhysXの初期化
	auto pPhysics = SCENE_MANAGER.GetPhysics();

	// 平面を追加
	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(
		*pPhysics, physx::PxPlane(0, 1, 0, 0),
		*pPhysics->createMaterial(0.5f, 0.5f, 0.5f)
	);

	// シーンに追加
	if (groundPlane)
	{
		// シーンに追加
		m_pScene->addActor(*groundPlane);
	}
	else
	{
		ELOG("Error : Plane creation failed.");
	}

	// テストアクターを追加
	TestActor* pActor = ACTOR_MANAGER.AddActor<TestActor>();
	auto body = pActor->AddComponent<RigidBodyComponent>();
	body->SetRigidBody(RigidBodyComponent::ActorType::Dynamic, RigidBodyComponent::ShapeType::Box);

	auto model = pActor->AddComponent<ModelComponent>();
	model->LoadModel("Assets/Models/material/material_test.obj");

	// ライトバッファの設定
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

	// カメラバッファの設定.
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

	// ルートシグネチャーの生成
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

	// パイプラインステートの生成
	{
		m_pScenePSO = NEW PipelineState();

		// 頂点シェーダの読み込み
		if (!SHADER_MANAGER.LoadShader("Assets/Shaders/VS_Basic.cso", "VS_Basic", ShaderManager::ShaderType::Vertex))
		{
			ELOG("[App.cpp]Error : Line222 : 頂点シェーダが見つかりませんでした。");
		}

		// ピクセルシェーダの読み込み
		if (!SHADER_MANAGER.LoadShader("Assets/Shaders/PS_Basic.cso", "PS_Basic", ShaderManager::ShaderType::Pixel))
		{
			ELOG("[App.cpp]Error : Line229 : ピクセルシェーダが見つかりませんでした。");
		}

		auto pVS = SHADER_MANAGER.GetShader("VS_Basic");
		auto pPS = SHADER_MANAGER.GetShader("PS_Basic");

		// パイプラインステートの設定
		PipelineState::Desc desc;
		desc.SetVertexShader({ pVS->GetBufferPointer(), pVS->GetBufferSize() })
			.SetPixelShader({ pPS->GetBufferPointer(), pPS->GetBufferSize() })
			.SetRootSignature(m_pSceneRootSig->GetPtr())
			.SetRenderTargetFormat(RENDERER.GetSceneColorTarget()->GetRTVDesc().Format, RENDERER.GetSceneDepthTarget()->GetDSVDesc().Format);

		// カスタム設定が必要な場合のみ設定を上書き
		// ラスタライザーステートの設定
		D3D12_RASTERIZER_DESC rsDesc = desc.GetDesc()->RasterizerState;
		rsDesc.FrontCounterClockwise = TRUE; // カスタム設定
		rsDesc.DepthClipEnable = FALSE; // カスタム設定
		desc.SetRasterizerState(rsDesc);

		// 入力レイアウトの設定
		D3D12_INPUT_ELEMENT_DESC elements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { elements, _countof(elements) };
		desc.SetInputLayout(inputLayoutDesc);

		// パイプラインステートの設定
		if (!m_pScenePSO->Init(desc.GetDesc()))
		{
			ELOG("Error : PipelineState::Init() Failed.");
		}
	}

	// 変換行列用の定数バッファの生成.
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{

			// 定数バッファ初期化.
			if (!m_TransformCB[i].Init(pPool, sizeof(CbTransform)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
			}

			// 変換行列を設定.
			auto ptr = m_TransformCB[i].GetPtr<CbTransform>();
			ptr->View = m_pCamera->GetView();
			ptr->Proj = m_pProjector->GetMatrix();
		}

		m_RotateAngle = DirectX::XMConvertToRadians(-60.0f);
	}

}

void TestScene::Uninit()
{
	// カメラの解放
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pProjector);

	// 定数バッファの解放
	for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_LightCB[i].Uninit();
		m_CameraCB[i].Uninit();
		m_TransformCB[i].Uninit();
	}

	// ルートシグネチャの解放
	m_pSceneRootSig->Uninit();
	SAFE_DELETE(m_pSceneRootSig);

	// パイプラインステートの解放
	m_pScenePSO->Uninit();
	SAFE_DELETE(m_pScenePSO);

	// シーンの解放
	if (m_pScene)
	{
		m_pScene->release();
		m_pScene = nullptr;
	}
}
