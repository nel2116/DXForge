// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 16:20
// �T�v   : RTVHeap�N���X�̒�`
// �X�V����
// 2021/05/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <wrl/client.h>


// ====== �N���X��` ======
class RTVHeap
{
private:
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// �����o�֐�
	RTVHeap() : m_useCount(0), m_incrementSize(0), m_nextIndex(0) {};
	~RTVHeap() = default;

	/// <summary>
	/// �q�[�v�쐬
	/// </summary>
	/// <param name="pDevice">�f�o�C�X�̃|�C���^</param>
	/// <param name="useCount">�g�p����RTV�̐�</param>
	/// <returns>����������true</returns>
	bool Create(ID3D12Device* pDevice, UINT useCount);

	/// <summary>
	/// RTV�̍쐬
	/// </summary>
	/// <param name="pBuffer">�o�b�t�@�̃|�C���^</param>
	/// <returns>�q�[�v�̕R�Â���ꂽ�o�^�ԍ�</returns>
	int CreateRTV(ID3D12Resource* pBuffer);

	/// <summary>
	/// RTV��CPU���A�h���X���擾
	/// </summary>
	/// <param name ="number">�o�^�ԍ�</param>
	/// <returns>RTV��GPU���A�h���X</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle(int number);

private:
	ID3D12Device* m_pDevice = nullptr;				// �f�o�C�X�̃|�C���^
	ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;	// �q�[�v�̃|�C���^
	int m_useCount;									// �g�p����RTV�̐�
	int m_incrementSize;							// �C���N�������g�T�C�Y
	int m_nextIndex;								// ���ɍ쐬����RTV�̔ԍ�
};