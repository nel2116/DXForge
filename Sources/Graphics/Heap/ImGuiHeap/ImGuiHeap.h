// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [imuGuiHeap.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/01 16:00
// �T�v   : imuGui�q�[�v�N���X�̒�`
// �X�V����
// 2024/06/01 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "../Heap.h"

class ImGuiHeap : public Heap<int>
{

public:
	ImGuiHeap() {}
	~ImGuiHeap() {}

	/// <summary>
	/// imuGui�q�[�v����
	/// </summary>
	/// <param name="pBuffer">�o�b�t�@�[�̃|�C���^</param>
	/// <param name="format">�t�H�[�}�b�g</param>
	/// <returns>�q�[�v�̕R�t����ꂽ�o�^�ԍ�</returns>
	int CreateimuGui(ID3D12Resource* pBuffer, DXGI_FORMAT format);

private:
};