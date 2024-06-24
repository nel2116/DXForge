// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [CommandList.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/24 13:05 : �쐬
// �T�v   : �R�}���h���X�g�N���X�̒�`
// �X�V����
// 2024/06/24  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/ComPtr.h>
#include <d3d12.h>
#include <cstdint>
#include <vector>

// ====== �N���X��` ======
class CommandList
{
public:
	// �R���X�g���N�^
	CommandList();

	// �f�X�g���N�^
	~CommandList();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="type">�R�}���h���X�g�̎��</param>
	/// <param name="count">�A���P�[�^�̐�</param>
	/// <returns>�������ɐ���������true</returns>
	bool Init(D3D12_COMMAND_LIST_TYPE type, size_t count);

	/// <summary>
	/// �I������
	/// </summary>
	void Uninit();

	/// <summary>
	/// �R�}���h���X�g�̎擾
	/// </summary>
	/// <returns>�R�}���h���X�g�̃|�C���^</returns>
	ID3D12GraphicsCommandList* Get() const;

	/// <summary>
	/// ���Z�b�g�������s�����R�}���h���X�g���擾
	/// </summary>
	/// <returns>�R�}���h���X�g�̃|�C���^</returns>
	ID3D12GraphicsCommandList* Reset();

private:	// �����o�ϐ�
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;					// �R�}���h���X�g
	std::vector<ComPtr<ID3D12CommandAllocator>> m_pAllocators;      // �R�}���h�A���P�[�^
	uint32_t m_Index;												// ���݂̃A���P�[�^�̃C���f�b�N�X

private:	// �R�s�[�֎~
	CommandList(CommandList const&) = delete;
	CommandList& operator=(CommandList const&) = delete;
};