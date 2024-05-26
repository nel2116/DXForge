// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 16:30
// 概要   : RTVHeapクラスの実装
// 更新履歴
// 2021/05/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "RTVHeap.h"

// ====== メンバ関数 ======
bool RTVHeap::Create(ID3D12Device* pDevice, UINT useCount)
{
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// レンダーターゲットとした使う
	desc.NodeMask = 0;
	desc.NumDescriptors = useCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// フラグは特になし

	auto hr = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeap));
	if (FAILED(hr))
	{
		// エラーメッセージ
		MessageBox(nullptr, "RTVHeapの生成に失敗しました", "RTVHeap Error", MB_OK);
		return false;
	}

	m_useCount = useCount;
	m_incrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_pDevice = pDevice;

	return true;
}

int RTVHeap::CreateRTV(ID3D12Resource* pBuffer)
{
	if (m_useCount < m_nextIndex)
	{
		// エラーメッセージ
		MessageBox(nullptr, "確保済みのヒープ領域を超えました。", "RTVHeap Error", MB_OK);
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_nextIndex * m_incrementSize;
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_pDevice->CreateRenderTargetView(pBuffer, &desc, handle);
	return m_nextIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVHeap::GetRTVCPUHandle(int number)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_incrementSize * number;
	return handle;
}
