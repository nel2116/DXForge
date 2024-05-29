// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Model.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 19:30
// �T�v   : ���f���N���X�̒�`
// �X�V���� 
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics/Mesh/Mesh.h>

class Model
{
public:		// �\���̒�`
	struct Node
	{
		std::shared_ptr<Mesh> spMesh;
	};

public:		// �p�u���b�N�֐�
	/// <summary>
	/// ���f���̃��[�h
	/// </summary>
	/// <param name="filePath">�t�@�C���p�X</param>
	/// <returns>���[�h�ɐ���������true</returns>
	bool Load(const std::string& filePath);

	/// <summary>
	/// �m�[�h�̎擾
	/// </summary>
	/// <returns>�m�[�h���</returns>
	const std::vector<Node>& GetNodes() const { return m_nodes; }

private:	// �����o�ϐ�
	std::vector<Node> m_nodes;
};