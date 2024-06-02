// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [imuGuiHeap.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/01 16:00
// 概要   : imuGuiヒープクラスの実装
// 更新履歴
// 2024/06/01 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "ImGuiHeap.h"

int ImGuiHeap::CreateimuGui(ID3D12Resource* pBuffer, DXGI_FORMAT format)
{
	if (m_useCount < m_nextRegisterNumber)
	{
		assert(0 && "確保済みのヒープ領域を超えました");
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_nextRegisterNumber * m_incrementSize;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_pRenderer->GetDev()->CreateDepthStencilView(pBuffer, &dsvDesc, handle);

	return m_nextRegisterNumber++;
}
