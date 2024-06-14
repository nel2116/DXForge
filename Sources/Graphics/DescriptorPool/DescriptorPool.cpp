// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/13 17:50
// 作成者 : 田中ミノル
// 概要   : ディスクリプタプールクラスの実装
// 更新履歴
// 2024/06/13 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "DescriptorPool.h"

DescriptorPool::DescriptorPool()
	: m_RefCount(1)
	, m_pHeap(nullptr)
	, m_Pool()
	, m_DescriptorSize(0)
{
}

DescriptorPool::~DescriptorPool()
{
	m_Pool.Uninit();
	m_pHeap.Reset();
	m_DescriptorSize = 0;
}

void DescriptorPool::AddRef()
{
	m_RefCount++;
}

void DescriptorPool::Release()
{
	m_RefCount--;
	if (m_RefCount == 0)
	{
		delete this;
	}
}

uint32_t DescriptorPool::GetCount() const
{
	return m_RefCount;
}

DescriptorHandle* DescriptorPool::AllocHandle()
{
	// 初期化関数
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
	{
		auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();

		handleCPU.ptr += m_DescriptorSize * index;
		handleGPU.ptr += m_DescriptorSize * index;

		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};

	// 初期化関数を実行してからハンドルを返す
	return m_Pool.Alloc(func);
}

void DescriptorPool::FreeHandle(DescriptorHandle*& pHandle)
{
	if (!pHandle)
	{
		// ハンドルをプールに戻す
		m_Pool.Free(pHandle);
		pHandle = nullptr;
	}
}

uint32_t DescriptorPool::GetAvailableHandleCount() const
{
	return m_Pool.GetAvailableCount();
}

uint32_t DescriptorPool::GetAllocatedHandleCount() const
{
	return m_Pool.GetUsedCount();
}

uint32_t DescriptorPool::GetHandleCount() const
{
	return m_Pool.GetSize();
}

ID3D12DescriptorHeap* const DescriptorPool::GetHeap() const
{
	return m_pHeap.Get();
}

bool DescriptorPool::Create(const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, DescriptorPool** ppPool)
{
	auto pDevice = RENDERER.GetDevice();
	// 引数チェック
	if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr)
	{
		return false;
	}

	// インスタンスを生成
	auto instance = NEW DescriptorPool();
	// インスタンスが生成できなかった
	if (!instance)
	{
		return false;
	}

	// ディスクリプタヒープの生成
	auto hr = pDevice->CreateDescriptorHeap(pDesc, IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));

	// 失敗したら開放処理を行ってから終了
	if (FAILED(hr))
	{
		instance->Release();
		return false;
	}

	// プールを初期化します。
	if (!instance->m_Pool.Init(pDesc->NumDescriptors))
	{
		instance->Release();
		return false;
	}

	// ディスクリプタサイズを取得
	instance->m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

	// インスタンスを設定
	*ppPool = instance;

	// 正常終了
	return true;
}