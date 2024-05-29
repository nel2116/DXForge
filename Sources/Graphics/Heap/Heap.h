// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Heap.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 12:00
// �T�v   : �e��q�[�v�N���X�̊��N���X��`
// �X�V����
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics\Renderer.h>

// ====== �񋓑� ======
enum class HeapType
{
	CBVSRVUAV = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	SAMPLER = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
	RTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
	DSV = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
};

// ====== �N���X��` ======
template<typename T>
class Heap
{
public:
	Heap() {};
	virtual ~Heap() {};

	/// <summary>
	/// �q�[�v�쐬
	/// </summary>
	/// <param name="pDev">�����_���[�N���X�̃|�C���^</param>
	/// <param name="type">�q�[�v�̎��</param>
	/// <param name="useCount">�g�p��</param>
	/// <returns>����������true</returns>
	bool Create(Renderer* pDev, HeapType type, T useCount)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
		desc.NodeMask = 0;
		desc.NumDescriptors = ComputUseCount(useCount);
		desc.Flags = type == HeapType::CBVSRVUAV ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (FAILED(pDev->GetDev()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeap))))
		{
			assert(0 && "�q�[�v�̍쐬�Ɏ��s���܂����B");
			return false;
		}

		m_useCount = useCount;
		m_incrementSize = pDev->GetDev()->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type));
		m_pRenderer = pDev;

		return true;
	}


	/// <summary>
	/// CPU���A�h���X��Ԃ�
	/// </summary>
	/// <param name="number">�o�^�ԍ�</param>
	/// <returns>CPU���A�h���X</returns>
	virtual const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int number)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * number;
		return handle;
	}

	/// <summary>
	/// GPU���A�h���X��Ԃ�
	/// </summary>
	/// <param name="number">�o�^�ԍ�</param>
	/// <returns>GPU���A�h���X</returns>
	virtual const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int number)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * number;
		return handle;
	}

protected:
	Renderer* m_pRenderer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;
	T m_useCount = {};
	int m_incrementSize = 0;
	int m_nextRegisterNumber = 0;

private:
	UINT ComputUseCount(UINT useCount)
	{
		return useCount;
	}

	UINT ComputUseCount(DirectX::XMFLOAT3 useCount)
	{
		return (UINT)(useCount.x + useCount.y + useCount.z);
	}

};