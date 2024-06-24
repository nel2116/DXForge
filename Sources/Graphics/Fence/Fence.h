// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Fence.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/14 12:05 : �쐬
// �T�v   : �t�F���X�N���X�̒�`
// �X�V����
// 2024/06/14  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/ComPtr.h>
#include <d3d12.h>

// ====== �N���X��` ======
class Fence
{
public:
	// �R���X�g���N�^
	Fence();
	// �f�X�g���N�^
	~Fence();

	/// <summary>
	/// ����������
	/// </summary>
	/// <returns>�������ɐ���������true</returns>
	bool Init();

	/// <summary>
	/// �I������
	/// </summary>
	void Uninit();

	/// <summary>
	/// �V�O�i����ԂɂȂ�܂Ŏw�肳�ꂽ���ԑҋ@
	/// </summary>
	/// <param name="time">�ҋ@����</param>
	void Wait(UINT timeout);

	/// <summary>
	/// �V�O�i����ԂɂȂ�܂őҋ@
	/// </summary>
	void Sync();

private:	// �����o�ϐ�
	ComPtr<ID3D12Fence> m_pFence;	// �t�F���X
	HANDLE m_hEvent;				// �C�x���g�n���h��
	UINT m_Counter;					// �t�F���X�̒l

private:
	Fence(Fence const&) = delete;
	Fence& operator=(Fence const&) = delete;
};