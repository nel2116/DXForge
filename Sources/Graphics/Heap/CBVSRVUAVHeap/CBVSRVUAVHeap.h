// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [CBVSRVUAVHeap.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 12:00
// �T�v   : CBV/SRV/UAV�q�[�v�N���X�̒�`
// �X�V����
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics\Heap\Heap.h>

// ====== �N���X�̒�` ======
class CBVSRVUAVHeap : public Heap<DirectX::XMFLOAT3>
{
public:
	CBVSRVUAVHeap() {}
	~CBVSRVUAVHeap() {}

	/// <summary>
	/// SRV�̍쐬
	/// </summary>
	/// <param name="pBuffer">�o�b�t�@�[�̃|�C���^</param>
	/// <returns>�q�[�v�̕R�Â���ꂽ�o�^�ԍ�</returns>
	int CreateSRV(ID3D12Resource* pBuffer);

	/// <summary>
	/// SRV��GPU���n���h����Ԃ�
	/// </summary>
	/// <param name="number">�o�^�ԍ�</param>
	/// <returns>GPU���n���h��</returns>
	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int number)override;

	/// <summary>
	/// �q�[�v���Z�b�g����
	/// </summary>
	void SetHeap();

	/// <summary>
/// �q�[�v�̎擾�֐�
/// </summary>
/// <returns></returns>
	ID3D12DescriptorHeap* GetHeap() { return m_pHeap.Get(); }

	/// <summary>
	/// �g�p�����擾
	/// </summary>
	/// <returns>�g�p��</returns>
	const DirectX::XMFLOAT3& GetUseCount() { return m_useCount; }
private:
};




