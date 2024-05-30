#include "stdafx.h"
#include "ModelLoader.h"

bool ModelLoader::Load(const std::string& filePath, std::vector<Model::Node>& nodes)
{
	Assimp::Importer importer;

	int flags = 0;
	flags |= aiProcess_CalcTangentSpace;
	flags |= aiProcess_Triangulate;
	flags |= aiProcess_ConvertToLeftHanded;
	flags |= aiProcess_GenSmoothNormals;
	flags |= aiProcess_PreTransformVertices;
	flags |= aiProcess_RemoveRedundantMaterials;
	flags |= aiProcess_GenUVCoords;
	flags |= aiProcess_OptimizeMeshes;

	const aiScene* pScene = importer.ReadFile(filePath, flags);

	if (pScene == nullptr)
	{
		assert(0 && "モデルのファイルが見つかりません。");
		return false;
	}

	nodes.resize(pScene->mNumMeshes);

	string dirPath = GetDirFromPath(filePath);

	for (UINT i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* pMesh = pScene->mMeshes[i];
		aiMaterial* pMaterial = pScene->mMaterials[i];
		nodes[i].spMesh = Parse(pScene, pMesh, pMaterial, dirPath);
	}

	return true;
}

std::shared_ptr<Mesh> ModelLoader::Parse(const aiScene* pScene, const aiMesh* pMesh, const aiMaterial* pMaterial, const std::string& dirPath)
{
	std::vector<MeshVertex> vertices;
	std::vector<MeshFace> faces;

	vertices.resize(pMesh->mNumVertices);

	for (UINT i = 0; i < pMesh->mNumVertices; ++i)
	{
		vertices[i].Pos.x = pMesh->mVertices[i].x;
		vertices[i].Pos.y = pMesh->mVertices[i].y;
		vertices[i].Pos.z = pMesh->mVertices[i].z;

		if (pMesh->HasTextureCoords(0))
		{
			vertices[i].UV.x = static_cast<float>(pMesh->mTextureCoords[0][i].x);
			vertices[i].UV.y = static_cast<float>(pMesh->mTextureCoords[0][i].y);

			// vertices[i].UV.x = 1.0f - vertices[i].UV.x;
			// vertices[i].UV.y = 1.0f - vertices[i].UV.y;
		}

		vertices[i].Normal.x = pMesh->mNormals[i].x;
		vertices[i].Normal.y = pMesh->mNormals[i].y;
		vertices[i].Normal.z = pMesh->mNormals[i].z;

		if (pMesh->HasTangentsAndBitangents())
		{
			vertices[i].Tangent.x = pMesh->mTangents[i].x;
			vertices[i].Tangent.y = pMesh->mTangents[i].y;
			vertices[i].Tangent.z = pMesh->mTangents[i].z;

			// vertices[i].BiNormal.x = pMesh->mBitangents[i].x;
			// vertices[i].BiNormal.y = pMesh->mBitangents[i].y;
			// vertices[i].BiNormal.z = pMesh->mBitangents[i].z;
		}

		if (pMesh->HasVertexColors(0))
		{
			DirectX::XMFLOAT4 color;

			color.x = pMesh->mColors[0][i].r;
			color.y = pMesh->mColors[0][i].g;
			color.z = pMesh->mColors[0][i].b;
			color.w = pMesh->mColors[0][i].a;

			vertices[i].Color = /*RGBA(color)*/0;
		}
	}

	faces.resize(pMesh->mNumFaces);

	for (UINT i = 0; i < pMesh->mNumFaces; ++i)
	{
		faces[i].idx[0] = pMesh->mFaces[i].mIndices[0];
		faces[i].idx[1] = pMesh->mFaces[i].mIndices[1];
		faces[i].idx[2] = pMesh->mFaces[i].mIndices[2];
	}

	std::shared_ptr<Mesh> spMesh = std::make_shared<Mesh>();
	spMesh->Create(&Renderer::Instance(), vertices, faces, ParseMaterial(pMaterial, dirPath));

	return spMesh;
}

const Material ModelLoader::ParseMaterial(const aiMaterial* pMaterial, const std::string& dirPath)
{
	Material material = {};

	// マテリアル名を取得
	{
		aiString name;
		if (pMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
		{
			material.name = name.C_Str();
		}
	}

	// Diffuseテクスチャの取得
	{
		aiString path;
		if (pMaterial->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &path) == AI_SUCCESS)
		{
			auto filePath = string(path.C_Str());
			material.spBaseColorTex = std::make_shared<Texture>();
			if (!material.spBaseColorTex->Load(&Renderer::Instance(), dirPath + filePath))
			{
				assert(0 && "Diffuseテクスチャの読み込みに失敗しました。");
				return Material();
			}
		}
	}

	//Diffuseカラーの取得
	{
		aiColor4D color;
		if (pMaterial->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS)
		{
			material.baseColor = XMFLOAT4(color.r, color.g, color.b, color.a);
		}
	}

	// MetallicRoughnessテクスチャの取得
	{
		aiString path;
		if (pMaterial->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &path) == AI_SUCCESS ||
			pMaterial->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &path) == AI_SUCCESS)
		{
			auto filePath = string(path.C_Str());
			material.spMetallicRoughnessTex = std::make_shared<Texture>();
			if (!material.spMetallicRoughnessTex->Load(&Renderer::Instance(), dirPath + filePath))
			{
				assert(0 && "MetallicRoughnessテクスチャの読み込みに失敗しました。");
				return Material();
			}
		}
	}

	// Metallicの取得
	{
		float metallic = 0.0f;
		if (pMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS)
		{
			material.metallic = metallic;
		}
	}

	// Roughnessの取得
	{
		float roughness = 0.0f;
		if (pMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS)
		{
			material.roughness = roughness;
		}
	}

	// Emissiveテクスチャの取得
	{
		aiString path;
		if (pMaterial->GetTexture(AI_MATKEY_EMISSIVE_TEXTURE, &path) == AI_SUCCESS)
		{
			auto filePath = string(path.C_Str());
			material.spEmissiveTex = std::make_shared<Texture>();
			if (!material.spEmissiveTex->Load(&Renderer::Instance(), dirPath + filePath))
			{
				assert(0 && "Emissiveテクスチャの読み込みに失敗しました。");
				return Material();
			}
		}
	}

	// Emissiveの取得
	{
		aiColor3D color;
		if (pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
		{
			material.emissive = XMFLOAT3(color.r, color.g, color.b);
		}
	}

	// Normalテクスチャの取得
	{
		aiString path;
		if (pMaterial->GetTexture(AI_MATKEY_NORMAL_TEXTURE, &path) == AI_SUCCESS)
		{
			auto filePath = string(path.C_Str());
			material.spNormalTex = std::make_shared<Texture>();
			if (!material.spNormalTex->Load(&Renderer::Instance(), dirPath + filePath))
			{
				assert(0 && "Normalテクスチャの読み込みに失敗しました。");
				return Material();
			}
		}
	}
	return material;
}
