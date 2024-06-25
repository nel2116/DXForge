//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Application.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : アプリケーションクラスの実装
// 更新履歴 : 2024/05/25 作成
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Application.h"
#include <Manager/ActorManager.h>

using namespace DirectX::SimpleMath;

struct alignas(256) CbMesh
{
	Matrix   World;      //!< ワールド行列です.
};

struct alignas(256) CbTransform
{
	Matrix   View;       //!< ビュー行列です.
	Matrix   Proj;       //!< 射影行列です.
};

struct alignas(256) CbLight
{
	Vector3  LightColor;        //!< ライトカラーです.
	float    LightIntensity;    //!< ライト強度です.
	Vector3  LightForward;      //!< ライトの照射方向です.
};

struct alignas(256) CbCamera
{
	Vector3  CameraPosition;    //!< カメラ位置です.
};

struct alignas(256) CbMaterial
{
	Vector3 BaseColor;  //!< 基本色.
	float   Alpha;      //!< 透過度.
	float   Roughness;  //!< 面の粗さです(範囲は[0,1]).
	float   Metallic;   //!< 金属度です(範囲は[0,1]).
};

// ====== メンバ関数 ======
bool Application::Init()
{
	SetDirectoryAndLoadDll();	// .dllのディレクトリのセットとロードを行う

	// COMの初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		MessageBox(nullptr, "COMの初期化に失敗しました。", "エラー", MB_OK);
		assert(0 && "COMの初期化に失敗しました。");
		return false;
	}

	if (!m_window.Create(WINDOW_WIDTH, WINDOW_HEIGHT, CLASS_NAME, WINDOW_NAME))
	{
		assert(0 && "ウィンドウの作成に失敗しました。");
		return false;
	}

	if (!RENDERER.Init(&m_window))
	{
		assert(0 && "Rendererの初期化に失敗しました。");
		return false;
	}

	// ====== Managerの初期化 ======
	ACTOR_MANAGER.Init();

	// メッシュをロード
	{
		std::wstring path;
		// ファイルパスを検索

		if (!SearchFilePathW(L"Assets/Models/material/material_test.obj", path))
		{
			ELOG("[App.cpp]Error : Line81 : ファイルが見つかりませんでした。");
			return false;
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// メッシュの読み込み
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line93 : メッシュの読み込みに失敗しました。FilePath = %ls", path.c_str());
			return false;
		}

		// メモリを予約
		m_pMesh.reserve(resMesh.size());

		// メッシュを初期化
		for (size_t i = 0; i < resMesh.size(); ++i)
		{
			// メッシュの生成
			auto pMesh = NEW Mesh();

			// チェック
			if (!pMesh)
			{
				ELOG("[App.cpp]Error : Line109 : メモリの確保に失敗しました。");
				return false;
			}

			// 初期化処理
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line116 : メッシュの初期化に失敗しました。");
				SAFE_DELETE(pMesh);
				return false;
			}

			// メッシュの追加
			m_pMesh.push_back(pMesh);
		}

		// メモリ最適化
		resMesh.shrink_to_fit();

		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);

		// マテリアルの初期化
		if (!m_Material.Init(pPool, sizeof(CbMaterial), resMaterial.size()))
		{
			ELOG("[App.cpp]Error : Line133 : マテリアルの初期化に失敗しました。");
			return false;
		}

		// テクスチャとマテリアルを設定.
		{
			/* ここではマテリアルが決め打ちであることを前提にハードコーディングしています. */
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

	// ライトバッファの設定
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

	// カメラバッファの設定.
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

	// シーン用カラーターゲットの生成
	{
		DescriptorPool* pRTVPool = RENDERER.GetPool(POOL_TYPE_RTV);
		DescriptorPool* pSRVPool = RENDERER.GetPool(POOL_TYPE_RES);

		float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		if (!m_SceneColorTarget.Init(pRTVPool, pSRVPool, WIDTH, HEIGHT, DXGI_FORMAT_R10G10B10A2_UNORM, clearColor))
		{
			ELOG("[Renderer.cpp]Error : Line 186 : ColorTarget.Init() Failed.");
			return false;
		}
	}

	// シーン用深度ターゲットの生成
	{
		DescriptorPool* pDSVPool = RENDERER.GetPool(POOL_TYPE_DSV);

		if (!m_SceneDepthTarget.Init(pDSVPool, nullptr, WIDTH, HEIGHT, DXGI_FORMAT_D32_FLOAT, 1.0f, 0))
		{
			ELOG("[Renderer.cpp]Error : Line 197 : DepthTarget.Init() Failed.");
			return false;
		}
	}

	// ルートシグネチャーの生成
	{
		RootSignature::Desc desc = {};
		desc.Begin(8)
			.SetCBV(ShaderStage::VS, 0, 0)
			.SetCBV(ShaderStage::PS, 1, 1)
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
			ELOG("[App.cpp]Error : Line223 : ルートシグネチャーの生成に失敗しました。");
			return false;
		}
	}

	// パイプラインステートの生成
	{
		std::wstring vsPath;
		std::wstring psPath;

		// 頂点シェーダを検索
		if (!SearchFilePathW(L"Assets/Shaders/VS_Basic.cso", vsPath))
		{
			ELOG("[App.cpp]Error : Line236 : 頂点シェーダが見つかりませんでした。");
			return false;
		}

		// ピクセルシェーダを検索
		if (!SearchFilePathW(L"Assets/Shaders/PS_Basic.cso", psPath))
		{
			ELOG("[App.cpp]Error : Line243 : ピクセルシェーダが見つかりませんでした。");
			return false;
		}

		ComPtr<ID3DBlob> pVS;
		ComPtr<ID3DBlob> pPS;

		// 頂点シェーダの読み込み
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVS.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line254 : 頂点シェーダの読み込みに失敗しました。");
			return false;
		}

		// ピクセルシェーダの読み込み
		hr = D3DReadFileToBlob(psPath.c_str(), pPS.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line262 : ピクセルシェーダの読み込みに失敗しました。");
			return false;
		}

		// ラスタライザステートの設定
		D3D12_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D12_FILL_MODE_SOLID;								// 頂点によって形作られている三角形で描画
		rsDesc.CullMode = D3D12_CULL_MODE_BACK;									// 背面カリング
		rsDesc.FrontCounterClockwise = TRUE;									// 反時計回りが表面
		rsDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;							// 使用しないのでデフォルト
		rsDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;					// 使用しないのでデフォルト
		rsDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;	// 使用しないのでデフォルト
		rsDesc.DepthClipEnable = FALSE;											// 使用しないのでFALSE
		rsDesc.MultisampleEnable = FALSE;										// 使用しないのでFALSE
		rsDesc.AntialiasedLineEnable = FALSE;									// 使用しないのでFALSE
		rsDesc.ForcedSampleCount = 0;											// 0 : サンプル数が強制されない
		rsDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF : 保守的なラスタライゼーションを無効にする

		// レンダーターゲットのブレンド設定(aブレンド)
		D3D12_RENDER_TARGET_BLEND_DESC rtDesc =
		{
			TRUE,FALSE,
			D3D12_BLEND_SRC_ALPHA,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		// ブレンドステートの設定
		D3D12_BLEND_DESC bsDesc = {};
		bsDesc.AlphaToCoverageEnable = FALSE;
		bsDesc.IndependentBlendEnable = FALSE;
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			bsDesc.RenderTarget[i] = rtDesc;
		}

		// 深度ステンシルステートの設定
		D3D12_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;

		// 入力レイアウトの設定
		D3D12_INPUT_ELEMENT_DESC elements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// パイプラインステートの設定
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
		desc.RTVFormats[0] = RENDERER.GetColorTarget(0)->GetRTVDesc().Format;
		desc.DSVFormat = RENDERER.GetDepthTarget()->GetDSVDesc().Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// パイプラインステートの生成
		hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pScenePSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line336 : パイプラインステートの生成に失敗しました。");
			return false;
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
				return false;
			}

			// カメラ設定.
			auto eyePos = Vector3(0.0f, 1.0f, 2.0f);
			auto targetPos = Vector3::Zero;
			auto upward = Vector3::UnitY;

			// 垂直画角とアスペクト比の設定.
			auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

			// 変換行列を設定.
			auto ptr = m_TransformCB[i].GetPtr<CbTransform>();
			ptr->View = Matrix::CreateLookAt(eyePos, targetPos, upward);
			ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
		}

		m_RotateAngle = DirectX::XMConvertToRadians(-60.0f);
	}

	// メッシュ用バッファの生成.
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

	// タイマーの初期化
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
			// ====== ここにゲームの処理を書く ======
			// ------ 更新処理 ------
			ACTOR_MANAGER.Update();

			// ------ 描画処理 ------
			RENDERER.Begin();

			// 描画
			{
				auto pCmd = RENDERER.GetCmdList()->Get();
				ID3D12DescriptorHeap* ppHeaps[] = { RENDERER.GetPool(POOL_TYPE_RES)->GetHeap(), };
				pCmd->SetDescriptorHeaps(1, ppHeaps);


				// ディスクリプタ取得
				auto handleRTV = m_SceneColorTarget.GetHandleRTV();
				auto handleDSV = m_SceneDepthTarget.GetHandleDSV();

				// 書き込み用リソースバリア設定
				RENDERER.TransitionResource(m_SceneColorTarget.GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

				// レンダーターゲットを設定
				pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

				// クリア
				m_SceneColorTarget.ClearView(pCmd);
				m_SceneDepthTarget.ClearView(pCmd);

				DrawScene();

				RENDERER.TransitionResource(m_SceneColorTarget.GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			}

			// --- 以下ゲームの描画処理
			ACTOR_MANAGER.Draw();

			// --- ここまでゲームの描画処理

			// ----- 描画終了 -----
			RENDERER.End(&m_SceneColorTarget);

			// ========================================
			m_dwFrameCount++;
		}
	}
}

void Application::Uninit()
{
	// ====== 終了処理 ======
	RENDERER.WaitGpu();

	// メッシュの解放
	for (auto& mesh : m_pMesh)
	{
		SAFE_UNINIT(mesh);
	}
	m_pMesh.clear();
	m_pMesh.shrink_to_fit();

	// マテリアルの解放
	m_Material.Uninit();

	// 定数バッファの解放
	for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_LightCB[i].Uninit();
		m_CameraCB[i].Uninit();
		m_TransformCB[i].Uninit();
		m_MeshCB[i].Uninit();
	}

	// シーン用カラーターゲットの解放
	m_SceneColorTarget.Uninit();

	// シーン用深度ターゲットの解放
	m_SceneDepthTarget.Uninit();

	// ルートシグネチャの解放
	m_SceneRootSig.Uninit();

	// パイプラインステートの解放
	m_pScenePSO.Reset();

	// ----- Managerの終了処理 -----
	ACTOR_MANAGER.Uninit();	// ActorManagerの終了処理

	// ----- システムの終了処理 -----
	RENDERER.Uninit();	// Rendererの終了処理
	timeEndPeriod(1);	// タイマーの終了処理
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// ウィンドウクラスの登録解除
	CoUninitialize();	// COMの終了処理
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

// シーンの描画
void Application::DrawScene()
{
	auto pCmd = RENDERER.GetCmdList()->Get();
	auto cameraPos = Vector3(-0.5f, 0.0f, 2.0f);
	auto frameIndex = RENDERER.GetFrameIndex();

	// ライトバッファの更新
	{
		auto matrix = Matrix::CreateRotationY(m_RotateAngle);

		auto ptr = m_LightCB[frameIndex].GetPtr<CbLight>();
		ptr->LightColor = Vector3(1.0f, 1.0f, 1.0f);
		ptr->LightForward = Vector3::TransformNormal(Vector3(0.0f, 1.0f, 1.0f), matrix);
		ptr->LightIntensity = 5.0f;
		m_RotateAngle += 0.01f;
	}
	// カメラバッファの更新.
	{
		auto ptr = m_CameraCB[frameIndex].GetPtr<CbCamera>();
		ptr->CameraPosition = cameraPos;
	}
	// メッシュのワールド行列の更新
	{
		auto ptr = m_MeshCB[frameIndex].GetPtr<CbMesh>();
		ptr->World = Matrix::Identity;
	}
	// 変換パラメータの更新
	{
		auto fovY = DirectX::XMConvertToRadians(37.5f);
		auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

		auto ptr = m_TransformCB[frameIndex].GetPtr<CbTransform>();
		ptr->View = Matrix::CreateLookAt(cameraPos, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
	}

	auto viewport = RENDERER.GetViewport();
	auto scissor = RENDERER.GetScissor();

	pCmd->SetGraphicsRootSignature(m_SceneRootSig.GetPtr());
	pCmd->SetGraphicsRootDescriptorTable(0, m_TransformCB[frameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(2, m_LightCB[frameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(3, m_CameraCB[frameIndex].GetHandleGPU());
	pCmd->SetPipelineState(m_pScenePSO.Get());
	pCmd->RSSetViewports(1, &viewport);
	pCmd->RSSetScissorRects(1, &scissor);

	// オブジェクトを描画.
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
		// マテリアルIDを取得.
		auto id = m_pMesh[i]->GetMaterialId();

		// テクスチャを設定.
		pCmd->SetGraphicsRootDescriptorTable(4, m_Material.GetTextureHandle(id, TU_BASE_COLOR));
		pCmd->SetGraphicsRootDescriptorTable(5, m_Material.GetTextureHandle(id, TU_METALLIC));
		pCmd->SetGraphicsRootDescriptorTable(6, m_Material.GetTextureHandle(id, TU_ROUGHNESS));
		pCmd->SetGraphicsRootDescriptorTable(7, m_Material.GetTextureHandle(id, TU_NORMAL));

		// メッシュを描画.
		m_pMesh[i]->Draw();
	}
}
