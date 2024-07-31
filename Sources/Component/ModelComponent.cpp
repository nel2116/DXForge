#include "stdafx.h"
#include "ModelComponent.h"

ModelComponent::~ModelComponent()
{
}

void ModelComponent::Init()
{
}

void ModelComponent::Update()
{
	int frameIndex = RENDERER.GetFrameIndex();

	// メッシュのワールド行列の更新
	{
		auto ptr = m_MeshCB[frameIndex].GetPtr<CbMesh>();
		ptr->World = Matrix::Identity;
	}
}

void ModelComponent::Draw()
{
	// モデルを描画.
	auto pCmd = RENDERER.GetCmdList()->Get();
	int frameIndex = RENDERER.GetFrameIndex();
	pCmd->SetGraphicsRootDescriptorTable(3, m_MeshCB[frameIndex].GetHandleGPU());
	for (size_t i = 0; i < m_pMesh.size(); ++i)
	{
		// マテリアルIDを取得.
		auto id = m_pMesh[i]->GetMaterialId();

		// テクスチャを設定.
		pCmd->SetGraphicsRootDescriptorTable(4, m_Material.GetTextureHandle(id, TU_DIFFUSE));

		// メッシュを描画.
		m_pMesh[i]->Draw();
	}
}

void ModelComponent::Uninit()
{
	// メッシュの解放
	for (auto& mesh : m_pMesh)
	{
		SAFE_UNINIT(mesh);
	}
	m_pMesh.clear();
	m_pMesh.shrink_to_fit();

	// マテリアルの解放
	m_Material.Uninit();
}

void ModelComponent::LoadModel(std::string fileName)
{
	// メッシュをロード
	{
		std::wstring path;
		std::wstring filePath = StringToWString(fileName);
		// ファイルパスを検索

		if (!SearchFilePathW(filePath.c_str(), path))
		{
			ELOG("[App.cpp]Error : Line81 : ファイルが見つかりませんでした。");
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// メッシュの読み込み
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line93 : メッシュの読み込みに失敗しました。FilePath = %ls", path.c_str());
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
			}

			// 初期化処理
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line116 : メッシュの初期化に失敗しました。");
				SAFE_DELETE(pMesh);
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
		}

		// テクスチャとマテリアルを設定.
		{
			for (size_t i = 0; i < resMaterial.size(); ++i)
			{
				auto ptr = m_Material.GetBufferPtr<CbMaterial>(i);
				ptr->BaseColor = resMaterial[i].Diffuse;
				ptr->Alpha = resMaterial[i].Alpha;
				ptr->Roughness = 0.2f;
				ptr->Metallic = 0.5f;

				std::wstring path = dir + resMaterial[i].DiffuseMap;
				m_Material.SetTexture(i, TU_DIFFUSE, path);
			}
		}
	}

	// メッシュ用バッファの生成.
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_MeshCB[i].Init(pPool, sizeof(CbMesh)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
			}

			auto ptr = m_MeshCB[i].GetPtr<CbMesh>();
			ptr->World = Matrix::Identity;
		}
	}
}
