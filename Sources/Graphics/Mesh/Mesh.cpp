// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 17:50
// 概要   : メッシュクラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Mesh.h"

// ====== メンバ関数 ======
void Mesh::Create(Renderer* pDev)
{
	// デバイスの取得
	m_pRenderer = pDev;

	// 頂点の座標
	{
		// 頂点の座標
		m_vertices.emplace_back(XMFLOAT3{ -0.75f,-0.75f, 0.0f });
		m_vertices.emplace_back(XMFLOAT3{ -0.75f, 0.75f, 0.0f });
		m_vertices.emplace_back(XMFLOAT3{ 0.75f, -0.75f, 0.0f });
		m_vertices.emplace_back(XMFLOAT3{ 0.75f, 0.75f, 0.0f });

		// 頂点バッファの作成
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = sizeof(XMFLOAT3) * m_vertices.size();
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		auto hr = m_pRenderer->GetDev()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBuff)
		);

		if (FAILED(hr)) { assert(0 && "頂点バッファーの作成に失敗しました。"); }

		m_vbView.BufferLocation = m_pVertexBuff->GetGPUVirtualAddress();
		m_vbView.SizeInBytes = resDesc.Width;		// 頂点バッファ全体のバイト数
		m_vbView.StrideInBytes = sizeof(XMFLOAT3);	// 1頂点あたりのバイト数
	}

	// インデックスデータ
	{
		m_indices.emplace_back(0);
		m_indices.emplace_back(1);
		m_indices.emplace_back(2);
		m_indices.emplace_back(3);


	}


	// マップ
	XMFLOAT3* vbMap = nullptr;
	m_pVertexBuff->Map(0, nullptr, (void**)&vbMap);
	copy(begin(m_vertices), end(m_vertices), vbMap);
	m_pVertexBuff->Unmap(0, nullptr);
}

void Mesh::DrawInstanced() const
{
	m_pRenderer->GetCmdList()->IASetVertexBuffers(0, 1, &m_vbView);
	m_pRenderer->GetCmdList()->DrawInstanced(4, 1, 0, 0);
}