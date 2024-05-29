// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Heap.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 12:00
// 概要   : 各種ヒープクラスの基底クラス定義
// 更新履歴
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics\Renderer.h>

// ====== 列挙体 ======
enum class HeapType
{
	CBVSRVUAV = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	SAMPLER = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
	RTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
	DSV = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
};

// ====== クラス定義 ======
template<typename T>
class Heap
{
public:
	Heap() {};
	virtual ~Heap() {};

	/// <summary>
	/// ヒープ作成
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	/// <param name="type">ヒープの種類</param>
	/// <param name="useCount">使用個数</param>
	/// <returns>成功したらtrue</returns>
	bool Create(Renderer* pDev, HeapType type, T useCount)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
		desc.NodeMask = 0;
		desc.NumDescriptors = ComputUseCount(useCount);
		desc.Flags = type == HeapType::CBVSRVUAV ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (FAILED(pDev->GetDev()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeap))))
		{
			assert(0 && "ヒープの作成に失敗しました。");
			return false;
		}

		m_useCount = useCount;
		m_incrementSize = pDev->GetDev()->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type));
		m_pRenderer = pDev;

		return true;
	}


	/// <summary>
	/// CPU側アドレスを返す
	/// </summary>
	/// <param name="number">登録番号</param>
	/// <returns>CPU側アドレス</returns>
	virtual const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int number)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * number;
		return handle;
	}

	/// <summary>
	/// GPU側アドレスを返す
	/// </summary>
	/// <param name="number">登録番号</param>
	/// <returns>GPU側アドレス</returns>
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