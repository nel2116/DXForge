// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/13 16:30
// �쐬�� : �c���~�m��
// �T�v   : �f�B�X�N���v�^�v�[���N���X�̒�`
// �X�V����
// 2024/06/13 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <d3d12.h>
#include <atomic>
#include <System/ComPtr.h>
#include <Graphics/Pool/Pool.h>

// ====== �N���X��` ======
class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPU�f�B�X�N���v�^�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPU�f�B�X�N���v�^�n���h��

	bool HasCPU() const { return HandleCPU.ptr != 0; }
	bool HasGPU() const { return HandleGPU.ptr != 0; }
};

class DescriptorPool
{
public:
	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="pDevice">�f�o�C�X</param>
	/// <param name="pDesc">�f�B�X�N���v�^�q�[�v�̍\���ݒ�</param>
	/// <param name="ppPool">�������ꂽ�f�B�X�N���v�^�v�[���̊i�[��</param>
	/// <returns>�����ɐ���������true</returns>
	static bool Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, DescriptorPool** ppPool);

	/// <summary>
	/// �Q�ƃJ�E���g�𑝂₷
	/// <summary>
	void AddRef();

	/// <summary>
	/// �J���������s��
	/// </summary>
	void Release();

	/// <summary>
	/// �Q�ƃJ�E���g�̎擾
	/// </summary>
	/// <returns>�Q�ƃJ�E���g</returns>
	uint32_t GetCount() const;

	/// <summary>
	/// �f�B�X�N���v�^�n���h����\�蓖�Ă�
	/// </summary>
	/// <returns>���蓖�Ă�ꂽ�f�B�X�N���v�^�n���h��</returns>
	DescriptorHandle* AllocHandle();

	/// <summary>
	/// �f�B�X�N���v�^�n���h�����J������
	/// </summary>
	/// <param name="pHandle">�J������f�B�X�N���v�^�n���h��</param>
	void FreeHandle(DescriptorHandle*& pHandle);

	/// <summary>
	///  ���p�\�ȃn���h�������擾����
	/// </summary>
	/// <returns>���p�\�ȃn���h����</returns>
	uint32_t GetAvailableHandleCount() const;

	/// <summary>
	/// ���蓖�čς݂̃n���h�������擾����
	/// </summary>
	/// <returns>���蓖�čς݂̃n���h����</returns>
	uint32_t GetAllocatedHandleCount() const;

	/// <summary>
	/// �n���h���������擾����
	/// </summary>
	/// <returns>�n���h������</returns>
	uint32_t GetHandleCount() const;

	/// <summary>
	/// �f�B�X�N���v�^�q�[�v���擾����
	/// </summary>
	/// <returns>�f�B�X�N���v�^�q�[�v</returns>
	ID3D12DescriptorHeap* const GetHeap() const;

private:	// �v���C�x�[�g�֐�
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	DescriptorPool();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~DescriptorPool();

	DescriptorPool(const DescriptorPool&) = delete;	// �R�s�[�R���X�g���N�^�̍폜
	void operator=(const DescriptorPool&) = delete;	// ������Z�q�̍폜

private:
	std::atomic<uint32_t> m_RefCount;	// �Q�ƃJ�E���^
	Pool<DescriptorHandle> m_Pool;	// �v�[��
	ComPtr<ID3D12DescriptorHeap> m_pHeap;	// �f�B�X�N���v�^�q�[�v
	uint32_t m_DescriptorSize;	// �f�B�X�N���v�^�T�C�Y

};