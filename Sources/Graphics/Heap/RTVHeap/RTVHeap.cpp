// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 12:00
// 概要   : RTVヒープクラスの実装
// 更新履歴
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "RTVHeap.h"

int RTVHeap::CreateRTV(ID3D12Resource* pBuffer)
{
	if (m_useCount < m_nextRegisterNumber)
	{
		assert(0 && "確保済みのヒープ領域を超えました。");
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_nextRegisterNumber * m_incrementSize;
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_pRenderer->GetDev()->CreateRenderTargetView(pBuffer, &desc, handle);
	return m_nextRegisterNumber++;
}
