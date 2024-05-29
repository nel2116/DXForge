// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [ModelLoader.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 19:30
// �T�v   : ���f�����[�_�[�N���X�̒�`
// �X�V���� 
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#ifdef _DEBUG
#pragma comment(lib,"assimp-vc143-mtd.lib")
#else
#pragma comment(lib,"assimp-vc143-mt.lib")	
#endif // _DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"

class ModelLoader
{
public:		// �p�u���b�N�֐�
	/// <summary>
	/// ���f���̃��[�h
	/// </summary>
	/// <param name="filePath">�t�@�C���p�X</param>
	/// <param name="nodes">�m�[�h���</param>
	/// <returns>���[�h�ɐ���������true</returns>
	bool Load(const std::string& filePath, std::vector<Model::Node>& nodes);

private:	// �v���C�x�[�g�֐�
	/// <summary>
	/// ���
	/// </summary>
	/// <param name="pScene">���f���V�[���̃|�C���^</param>
	/// <param name="pMesh">���b�V���̃|�C���^</param>
	/// <param name="pMaterial">�}�e���A���̃|�C���^</param>
	/// <param name="dirPath">�f�B���N�g���p�X</param>
	/// <returns>���b�V���|�C���^</returns>
	std::shared_ptr<Mesh> Parse(const aiScene* pScene, const aiMesh* pMesh, const aiMaterial* pMaterial, const std::string& dirPath);

	/// <summary>
	/// �}�e���A���̉��
	/// </summary>
	/// <param name="pMaterial">�}�e���A���̃|�C���^</param>
	/// <param name="dirPath">�f�B���N�g���p�X</param>
	/// <returns>�}�e���A�����</returns>
	const Material ParseMaterial(const aiMaterial* pMaterial, const std::string& dirPath);

};