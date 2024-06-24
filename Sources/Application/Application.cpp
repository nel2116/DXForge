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

struct Transform
{
	Matrix World;		// ワールド行列
	Matrix View;		// ビュー行列
	Matrix Proj;		// 射影行列
};

struct LigBuffer
{
	Vector4 LigPos;		// ライトの位置
	Color LigColor;		// ライトの色
	Vector4 CameraPos;	// カメラの位置
};

struct MaterialBufer
{
	Vector3 BaseColor;	// 拡散反射率
	float Alpha;		// 透過度
	float Metallic;		// 金属度
	float Shininess;	// 鏡面反射強度
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

		if (!SearchFilePathW(L"Assets/Models/Cube/Cube.gltf", path))
		{
			ELOG("[App.cpp]Error : Line72 : ファイルが見つかりませんでした。");
			return false;
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// メッシュの読み込み
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line72 : メッシュの読み込みに失敗しました。FilePath = %ls", path.c_str());
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
				ELOG("[App.cpp]Error : Line88 : メモリの確保に失敗しました。");
				return false;
			}

			// 初期化処理
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line96 : メッシュの初期化に失敗しました。");
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
		if (!m_Material.Init(pPool, sizeof(MaterialBufer), resMaterial.size()))
		{
			ELOG("[App.cpp]Error : Line111 : マテリアルの初期化に失敗しました。");
			return false;
		}

		// テクスチャとマテリアルの設定
		for (size_t i = 0; i < resMaterial.size(); ++i)
		{
			auto ptr = m_Material.GetBufferPtr<MaterialBufer>(i);
			ptr->BaseColor = resMaterial[i].Diffuse;
			ptr->Alpha = resMaterial[i].Alpha;
			ptr->Metallic = 0.5f;
			ptr->Shininess = resMaterial[i].Shininess;

			// テクスチャの設定
			std::wstring path = dir + resMaterial[i].DiffuseMap;
			m_Material.SetTexture(i, TU_DIFFUSE, path);
		}

		// 使い終わったので解放
		pPool->Release();
	}

	// ライトバッファの設定
	{
		auto pCB = NEW ConstantBuffer();
		// チェック
		if (!pCB)
		{
			ELOG("[App.cpp]Error : Line136 : メモリの確保に失敗しました。");
			return false;
		}

		// 初期化
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		if (!pCB->Init(pPool, sizeof(LigBuffer)))
		{
			ELOG("[App.cpp]Error : Line144 : ライトバッファの初期化に失敗しました。");
			SAFE_DELETE(pCB);
			return false;
		}

		auto ptr = pCB->GetPtr<LigBuffer>();
		ptr->LigPos = Vector4(0.0f, 50.0f, 100.0f, 0.0f);
		ptr->LigColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
		ptr->CameraPos = Vector4(0.0f, 1.0f, 2.0f, 0.0f);

		m_pLight = pCB;

		// 使い終わったので解放
		pPool->Release();
	}

	// ルートシグネチャーの生成
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		// ルートパラメーターの設定
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// ディスクリプタレンジを設定
		D3D12_DESCRIPTOR_RANGE range = {};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = 0;

		// ルートパラメーターを設定
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

		// スタティックサンプラーの設定
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

		// ルートシグネチャーの設定
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = _countof(param);
		desc.NumStaticSamplers = 1;
		desc.pParameters = param;
		desc.pStaticSamplers = &sampler;
		desc.Flags = flag;

		// ルートシグネチャーの生成
		ComPtr<ID3DBlob> pBlob;
		ComPtr<ID3DBlob> pError;

		// シリアライズ
		auto hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, pBlob.GetAddressOf(), pError.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line257 : ルートシグネチャーのシリアライズに失敗しました。");
			return false;
		}

		// ルートシグネチャーの生成
		hr = RENDERER.GetDevice()->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(m_pRootSig.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line265 : ルートシグネチャーの生成に失敗しました。");
			return false;
		}
	}

	// パイプラインステートの生成
	{
		std::wstring vsPath;
		std::wstring psPath;

		// 頂点シェーダを検索
		if (!SearchFilePathW(L"Assets/Shaders/VS_Phong.cso", vsPath))
		{
			ELOG("[App.cpp]Error : Line245 : 頂点シェーダが見つかりませんでした。");
			return false;
		}

		// ピクセルシェーダを検索
		if (!SearchFilePathW(L"Assets/Shaders/PS_Phong.cso", psPath))
		{
			ELOG("[App.cpp]Error : Line253 : ピクセルシェーダが見つかりませんでした。");
			return false;
		}

		ComPtr<ID3DBlob> pVS;
		ComPtr<ID3DBlob> pPS;

		// 頂点シェーダの読み込み
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVS.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line272 : 頂点シェーダの読み込みに失敗しました。");
			return false;
		}

		// ピクセルシェーダの読み込み
		hr = D3DReadFileToBlob(psPath.c_str(), pPS.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line280 : ピクセルシェーダの読み込みに失敗しました。");
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

		// パイプラインステートの設定
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

		// パイプラインステートの生成
		hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[App.cpp]Error : Line345 : パイプラインステートの生成に失敗しました。");
			return false;
		}
	}

	// 変換行列用の定数バッファの生成
	{
		m_Transform.reserve(FRAME_BUFFER_COUNT);

		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{
			auto pCB = NEW ConstantBuffer();
			// チェック
			if (!pCB)
			{
				ELOG("[App.cpp]Error : Line360 : メモリの確保に失敗しました。");
				return false;
			}

			// 初期化
			DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
			if (!pCB->Init(pPool, sizeof(Transform) * 2))
			{
				ELOG("[App.cpp]Error : Line376 : 変換行列用の定数バッファの初期化に失敗しました。");
				SAFE_DELETE(pCB);
				return false;
			}

			// カメラ設定.
			auto eyePos = Vector3(0.0f, 1.0f, 10.0f);
			auto targetPos = Vector3::Zero;
			auto upward = Vector3::UnitY;

			// 垂直画角とアスペクト比の設定.
			auto constexpr fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

			// 変換行列を設定.
			auto ptr = pCB->GetPtr<Transform>();
			ptr->World = Matrix::Identity;
			ptr->View = Matrix::CreateLookAt(eyePos, targetPos, upward);
			ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 1.0f, 1000.0f);

			m_Transform.push_back(pCB);
		}
		m_RotateAngle = DirectX::XMConvertToRadians(-60.0f);
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

			auto frameIndex = RENDERER.GetFrameIndex();
			// teapotの更新
			{
				m_RotateAngle += 0.025f;

				auto pTransform = m_Transform[frameIndex]->GetPtr<Transform>();
				pTransform->World = Matrix::CreateRotationY(m_RotateAngle);
			}

			// teapotの描画
			{
				auto pCmdList = RENDERER.GetCmdList()->Get();

				ID3D12DescriptorHeap* const pHeaps[] = { RENDERER.GetPool(POOL_TYPE_RES)->GetHeap() };

				// パイプラインステートの設定
				pCmdList->SetGraphicsRootSignature(m_pRootSig.Get());
				pCmdList->SetDescriptorHeaps(1, pHeaps);
				pCmdList->SetGraphicsRootConstantBufferView(0, m_Transform[frameIndex]->GetAddress());
				pCmdList->SetGraphicsRootConstantBufferView(1, m_pLight->GetAddress());
				pCmdList->SetPipelineState(m_pPSO.Get());
				auto viewport = RENDERER.GetViewport();
				auto scissor = RENDERER.GetScissor();
				pCmdList->RSSetViewports(1, &viewport);
				pCmdList->RSSetScissorRects(1, &scissor);

				// メッシュの描画
				for (size_t i = 0; i < m_pMesh.size(); ++i)
				{
					// マテリアルの設定
					auto id = m_pMesh[i]->GetMaterialId();

					// 定数バッファを設定
					pCmdList->SetGraphicsRootConstantBufferView(2, m_Material.GetBufferAddress(i));

					// テクスチャを設定
					auto pTexture = m_Material.GetTextureHandle(id, TU_DIFFUSE);
					pCmdList->SetGraphicsRootDescriptorTable(3, pTexture);

					// 描画
					m_pMesh[i]->Draw();
				}

			}

			// --- 以下ゲームの描画処理
			ACTOR_MANAGER.Draw();

			// --- ここまでゲームの描画処理

			// ----- 描画終了 -----
			RENDERER.End();
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

	// ライトバッファの解放
	SAFE_DELETE(m_pLight);

	// 変換行列用の定数バッファの開放
	for (auto& cb : m_Transform)
	{
		SAFE_UNINIT(cb);
	}
	m_Transform.clear();
	m_Transform.shrink_to_fit();

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
