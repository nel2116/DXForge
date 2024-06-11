//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 19:00
// �T�v   : ���b�V���N���X�̎���
// �X�V���� : 2024/06/11 �쐬
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Mesh.h"

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
	// UTF-16����UTF-8�ɕϊ�
	std::string ToUTF8(const wstring& text)
	{
		auto length = WideCharToMultiByte(CP_UTF8, 0U, text.data(), -1, nullptr, 0, nullptr, nullptr);
		auto buffer = NEW char[length];

		WideCharToMultiByte(CP_UTF8, 0U, text.data(), -1, buffer, length, nullptr, nullptr);

		std::string result(buffer);
		SAFE_DELETE_ARRAY(buffer);

		return result;
	}

	class MeshLoader
	{
	public:
		MeshLoader() {}
		~MeshLoader() {}

		bool Load(const wchar_t* fileName, std::vector<Mesh>& meshes, std::vector<Material>& materials);

	private:
		void ParseMesh(Mesh& dstMesh, const aiMesh* pSrcMesh);
		void ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial);

	};

	// ���b�V���̓ǂݍ���
	bool MeshLoader::Load(const wchar_t* fileName, std::vector<Mesh>& meshes, std::vector<Material>& materials)
	{
		if (!fileName) { return false; }

		// wchar_t����char(UTF-8)�ɕϊ�
		auto path = ToUTF8(fileName);

		Assimp::Importer importer;
		int flags = 0;
		flags |= aiProcess_Triangulate;					// ���ׂẴ��b�V���̂��ׂĂ̖ʃf�[�^���O�p�`�ɕϊ�
		flags |= aiProcess_PreTransformVertices;		// �m�[�h�t���O���폜���A�����̃m�[�h�������[�J���ϊ��s��ł��ׂĂ̒��_�����O�ɕϊ����܂��B
		flags |= aiProcess_CalcTangentSpace;			// �C���|�[�g���ꂽ���b�V���ɑ΂��Đڐ��x�N�g���Ə]�ڐ��x�N�g�����v�Z���܂��B
		flags |= aiProcess_GenSmoothNormals;			// ���b�V�����̂��ׂĂ̒��_�ɂ��ăX���[�X�@���x�N�g���𐶐����܂��B
		flags |= aiProcess_GenUVCoords;					// ���b�V�����̂��ׂĂ̒��_�Ƀe�N�X�`�����W�𐶐����܂��B
		flags |= aiProcess_RemoveRedundantMaterials;	// �V�[�����̂��ׂẴ}�e���A�����������A�g�p����Ă��Ȃ��}�e���A�����폜���܂��B
		flags |= aiProcess_OptimizeMeshes;				// ���b�V�������œK�����܂��B

		// �t�@�C����ǂݍ���
		auto pScene = importer.ReadFile(path, flags);

		// �`�F�b�N
		if (!pScene)
		{
			assert(0 && "[Mesh.cpp]�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B");
			return false;
		}

		// ���b�V���̃��������m��
		meshes.clear();
		meshes.resize(pScene->mNumMeshes);

		// ���b�V���f�[�^��ϊ�
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			const auto pMesh = pScene->mMeshes[i];
			ParseMesh(meshes[i], pMesh);
		}

		// �}�e���A���̃��������m��
		materials.clear();
		materials.resize(pScene->mNumMaterials);

		// �}�e���A���f�[�^��ϊ�
		for (size_t i = 0; i < materials.size(); ++i)
		{
			const auto pMaterial = pScene->mMaterials[i];
			ParseMaterial(materials[i], pMaterial);
		}

		//�s�v�ɂȂ����̂ŃN���A
		pScene = nullptr;

		// ����I��
		return true;
	}

	void MeshLoader::ParseMesh(Mesh& dstMesh, const aiMesh* pSrcMesh)
	{
		// �}�e���A���ԍ���ݒ�
		dstMesh.materialIdx = pSrcMesh->mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// ���_�f�[�^�̃��������m��
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

		// �C���f�b�N�X�f�[�^�̃��������m��
		dstMesh.indices.resize(pSrcMesh->mNumFaces * 3);

		for (auto i = 0u; i < pSrcMesh->mNumFaces; ++i)
		{
			const auto& pFace = pSrcMesh->mFaces[i];
			assert(pFace.mNumIndices == 3);		// �O�p�`�̂ݑΉ�

			dstMesh.indices[i * 3 + 0] = pFace.mIndices[0];
			dstMesh.indices[i * 3 + 1] = pFace.mIndices[1];
			dstMesh.indices[i * 3 + 2] = pFace.mIndices[2];
		}
	}

	void MeshLoader::ParseMaterial(Material& dstMaterial, const aiMaterial* pSrcMaterial)
	{
		// �g�U���ː���
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

		// ���ʔ��ː���
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

		// ���ʔ��ˋ��x
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

		// �f�B�t���[�Y�}�b�v
		{
			aiString path;
			if (pSrcMaterial->Get(AI_MATKEY_TEXBLEND_DIFFUSE(0), path) == AI_SUCCESS)
			{
				dstMaterial.DiffuseMap = path.C_Str();
			}
			else
			{
				dstMaterial.DiffuseMap.clear();
			}
		}

	}
}	// namespace

// ====== �萔�E�}�N����` ======
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

bool LoadMesh(const wchar_t* fileName, std::vector<Mesh>& meshes, std::vector<Material>& materials)
{
	MeshLoader loader;
	return loader.Load(fileName, meshes, materials);
}
