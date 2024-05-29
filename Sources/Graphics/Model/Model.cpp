// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Model.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 19:30
// �T�v   : ���f���N���X�̒�`
// �X�V���� 
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Model.h"
#include "ModelLoader.h"

bool Model::Load(const std::string& filePath)
{
	ModelLoader modelLoader;
	if (!modelLoader.Load(filePath, m_nodes))
	{
		assert(0 && "���f���̃��[�h�Ɏ��s���܂����B");
		return false;
	}
	return true;
}
