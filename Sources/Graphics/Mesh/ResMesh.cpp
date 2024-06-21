//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 19:00
// 概要   : メッシュクラスの実装
// 更新履歴 : 2024/06/11 作成
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "stdafx.h"
#include "ResMesh.h"

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif // _DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <codecvt>
#include <cassert>

namespace
{
	// UTF-16からUTF-8に変換
	std::string ToUTF8(const wstring& text)
	{
		auto length = WideCharToMultiByte(CP_UTF8, 0U, text.data(), -1, nullptr, 0, nullptr, nullptr);
		auto buffer = NEW char[length];

		WideCharToMultiByte(CP_UTF8, 0U, text.data(), -1, buffer, length, nullptr, nullptr);

		std::string result(buffer);
		SAFE_DELETE_ARRAY(buffer);

		return result;
	}

	std::wstring Convert(const aiString& path)
	{
		wchar_t temp[256] = {};
		size_t  size;
		mbstowcs_s(&size, temp, path.C_Str(), 256);
		return std::wstring(temp);
	}

	class MeshLoader
	{
	public:
		MeshLoader() {}
		~MeshLoader() {}

		bool Load(const wchar_t* fileName, std::vector<ResMesh>& meshes, std::vector<ResMaterial>& materials);

	private:
		const aiScene* m_pScene = nullptr;	// シーンデータ
		void ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh);
		void ParseMaterial(ResMaterial& dstMaterial, const aiMaterial* pSrcMaterial);

	};

	// メッシュの読み込み
	bool MeshLoader::Load(const wchar_t* fileName, std::vector<ResMesh>& meshes, std::vector<ResMaterial>& materials)
	{
		if (!fileName) { return false; }

		// wchar_tからchar(UTF-8)に変換
		auto path = ToUTF8(fileName);

		Assimp::Importer importer;
		int flags = 0;
		flags |= aiProcess_Triangulate;					// すべてのメッシュのすべての面データを三角形に変換
		flags |= aiProcess_PreTransformVertices;		// ノードフラグを削除し、それらのノードが持つローカル変換行列ですべての頂点を事前に変換します。
		flags |= aiProcess_CalcTangentSpace;			// インポートされたメッシュに対して接線ベクトルと従接線ベクトルを計算します。
		flags |= aiProcess_GenSmoothNormals;			// メッシュ内のすべての頂点についてスムース法線ベクトルを生成します。
		flags |= aiProcess_GenUVCoords;					// メッシュ内のすべての頂点にテクスチャ座標を生成します。
		flags |= aiProcess_RemoveRedundantMaterials;	// シーン内のすべてのマテリアルを検査し、使用されていないマテリアルを削除します。
		flags |= aiProcess_OptimizeMeshes;				// メッシュ数を最適化します。

		// ファイルを読み込む
		m_pScene = importer.ReadFile(path, flags);

		// チェック
		if (!m_pScene)
		{
			assert(0 && "[Mesh.cpp]ファイルの読み込みに失敗しました。");
			return false;
		}

		// メッシュのメモリを確保
		meshes.clear();
		meshes.resize(m_pScene->mNumMeshes);

		// メッシュデータを変換
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			const auto pMesh = m_pScene->mMeshes[i];
			ParseMesh(meshes[i], pMesh);
		}

		// マテリアルのメモリを確保
		materials.clear();
		materials.resize(m_pScene->mNumMaterials);

		// マテリアルデータを変換
		for (size_t i = 0; i < materials.size(); ++i)
		{
			const auto pMaterial = m_pScene->mMaterials[i];
			ParseMaterial(materials[i], pMaterial);
		}

		//不要になったのでクリア				
		importer.FreeScene();
		m_pScene = nullptr;

		// 正常終了
		return true;
	}

	void MeshLoader::ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh)
	{
		// マテリアル番号を設定
		dstMesh.materialIdx = pSrcMesh->mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// 頂点データのメモリを確保
		dstMesh.vertices.resize(pSrcMesh->mNumVertices);

		for (auto i = 0u; i < pSrcMesh->mNumVertices; ++i)
		{
			auto pPos = &(pSrcMesh->mVertices[i]);
			auto pNormal = &(pSrcMesh->mNormals[i]);
			auto pTexCoord = pSrcMesh->HasTextureCoords(0) ? &(pSrcMesh->mTextureCoords[0][i]) : &zero3D;
			auto pTangent = pSrcMesh->HasTangentsAndBitangents() ? &(pSrcMesh->mTangents[i]) : &zero3D;

			dstMesh.vertices[i] = MeshVertex(
				DirectX::XMFLOAT3(pPos->x, pPos->y, pPos->z),
				DirectX::XMFLOAT3(pNormal->x, pNormal->y, pNormal->z),
				DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y),
				DirectX::XMFLOAT3(pTangent->x, pTangent->y, pTangent->z));
		}

		// インデックスデータのメモリを確保
		dstMesh.indices.resize(pSrcMesh->mNumFaces * 3);

		for (auto i = 0u; i < pSrcMesh->mNumFaces; ++i)
		{
			const auto& pFace = pSrcMesh->mFaces[i];
			assert(pFace.mNumIndices == 3);		// 三角形のみ対応

			dstMesh.indices[i * 3 + 0] = pFace.mIndices[0];
			dstMesh.indices[i * 3 + 1] = pFace.mIndices[1];
			dstMesh.indices[i * 3 + 2] = pFace.mIndices[2];
		}
	}

	void MeshLoader::ParseMaterial(ResMaterial& dstMaterial, const aiMaterial* pSrcMaterial)
	{
		// 透過度
		{
			float alpha = 0.0f;
			if (pSrcMaterial->Get(AI_MATKEY_OPACITY, alpha) == AI_SUCCESS)
			{
				dstMaterial.Alpha = alpha;
			}
			else
			{
				dstMaterial.Alpha = 0.0f;
			}

		}

		// 拡散反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if (pSrcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				dstMaterial.Diffuse = DirectX::XMFLOAT3(color.r, color.g, color.b);
			}
			else
			{
				dstMaterial.Diffuse = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
			}
		}

		// 鏡面反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if (pSrcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			{
				dstMaterial.Specular = DirectX::XMFLOAT3(color.r, color.g, color.b);
			}
			else
			{
				dstMaterial.Specular = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
			}
		}

		// 鏡面反射強度
		{
			float shininess = 0.0f;

			if (pSrcMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{
				dstMaterial.Shininess = shininess;
			}
			else
			{
				dstMaterial.Shininess = 0.0f;
			}
		}

		// ディフューズマップ
		{
			aiString path;
			if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
			{
				dstMaterial.DiffuseMap = Convert(path);
			}
			else
			{
				dstMaterial.DiffuseMap.clear();
			}
		}

		// スペキュラーマップ
		{
			aiString path;
			if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == AI_SUCCESS)
			{
				dstMaterial.SpecularMap = Convert(path);
			}
			else
			{
				dstMaterial.SpecularMap.clear();
			}
		}

		// シャイネスマップ
		{
			aiString path;
			if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_SHININESS(0), path) == AI_SUCCESS)
			{
				dstMaterial.ShininessMap = Convert(path);
			}
			else
			{
				dstMaterial.ShininessMap.clear();
			}
		}

		// 法線マップ
		{
			aiString path;
			if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == AI_SUCCESS)
			{
				dstMaterial.NormalMap = Convert(path);
			}
			else
			{
				if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_HEIGHT(0), path) == AI_SUCCESS)
				{
					dstMaterial.NormalMap = Convert(path);
				}
				else
				{
					dstMaterial.NormalMap.clear();
				}
			}
		}
	}
}	// namespace

// ====== 定数・マクロ定義 ======
#define FMT_FLOAT3      DXGI_FORMAT_R32G32B32_FLOAT
#define FMT_FLOAT2      DXGI_FORMAT_R32G32_FLOAT
#define APPEND          D3D12_APPEND_ALIGNED_ELEMENT
#define IL_VERTEX       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA

const D3D12_INPUT_ELEMENT_DESC MeshVertex::InputElements[] =
{
	{ "POSITION", 0, FMT_FLOAT3, 0, APPEND, IL_VERTEX, 0 },
	{ "NORMAL",   0, FMT_FLOAT3, 0, APPEND, IL_VERTEX, 0 },
	{ "TEXCOORD", 0, FMT_FLOAT2, 0, APPEND, IL_VERTEX, 0 },
	{ "TANGENT",  0, FMT_FLOAT3, 0, APPEND, IL_VERTEX, 0 },
};

const D3D12_INPUT_LAYOUT_DESC MeshVertex::InputLayout = {
	MeshVertex::InputElements,
	MeshVertex::InputElementCount
};

static_assert(sizeof(MeshVertex) == 44, "Vertex struct/layout mismatch");

#undef FMT_FLOAT3
#undef FMT_FLOAT2
#undef APPEND
#undef IL_VERTEX

bool LoadMesh(const wchar_t* fileName, std::vector<ResMesh>& meshes, std::vector<ResMaterial>& materials)
{
	MeshLoader loader;
	return loader.Load(fileName, meshes, materials);
}
