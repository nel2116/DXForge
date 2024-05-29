// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [MeshData.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 19:00
// �T�v   : ���b�V���f�[�^�̐錾
// �X�V����
// 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once

// ���b�V���̒��_���
struct MeshVertex
{
	DirectX::XMFLOAT3 Pos;				// �ʒu���W
	DirectX::XMFLOAT2 UV;				// �e�N�X�`�����W
	DirectX::XMFLOAT3 Normal;			// �@��
	UINT Color = 0xFFFFFFFF;	// �F
	DirectX::XMFLOAT3 Tangent;			// �ڐ�
};