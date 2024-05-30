// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [CBufferAllocater.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 17:00
// 概要   : 定数バッファのアロケーター
// 更新履歴 
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "CBufferAllocater.h"

void CBufferAllocater::Create(Renderer* pDev, CBVSRVUAVHeap* pHeap)
{
	m_pRenderer = pDev;
	m_pHeap = pHeap;

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Width = (UINT64)((1 + 0xff) & ~0xff) * (int)m_pHeap->GetUseCount().x;
	HRESULT result = m_pRenderer->GetDev()->CreateCommittedResource
	(&heapprop, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pBuffer));

	if (FAILED(result))
	{
		assert(0 && "CBufferAllocaterの作成失敗");
		return;
	}

	m_pBuffer->Map(0, nullptr, (void**)&m_pMappedBuffer);
}

void CBufferAllocater::ResetNumber()
{
	m_currentUseNumber = 0;
}
