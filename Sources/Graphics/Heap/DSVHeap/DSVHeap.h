// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DSVHeap.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/30 09:30
// �T�v   : �[�x�X�e���V���q�[�v�N���X�̒�`
// �X�V����
// 2024/05/30 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "../Heap.h"

class DSVHeap : public Heap<int>
{
public:
	DSVHeap() {}
	~DSVHeap() {}

	/// <summary>
	/// �[�x�X�e���V���q�[�v����
	/// </summary>
	/// <param name="pBuffer">�o�b�t�@�[�̃|�C���^</param>
	/// <param name="format">�t�H�[�}�b�g</param>
	/// <returns>�q�[�v�̕R�t����ꂽ�o�^�ԍ�</returns>
	int CreateDSV(ID3D12Resource* pBuffer, DXGI_FORMAT format);
private:
};