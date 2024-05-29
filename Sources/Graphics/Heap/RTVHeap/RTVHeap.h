// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 12:00
// �T�v   : RTV�q�[�v�N���X�̒�`
// �X�V����
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics/Heap/Heap.h>

// ====== �N���X��` ======
class RTVHeap : public Heap<int>
{
public:
	RTVHeap() {};
	~RTVHeap() {};

	/// <summary>
	/// RTV�̍쐬
	/// </summary>
	/// <param name="pBuffer">�o�b�t�@�[�̃|�C���^</param>
	/// <returns>�q�[�v�̕R�Â���ꂽ�o�^�ԍ�</returns>
	int CreateRTV(ID3D12Resource* pBuffer);

private:
};
