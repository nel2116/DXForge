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
void Mesh::Create(Renderer* pDev, const std::vector<MeshVertex>& vertices, const std::vector<MeshFace>& faces, const Material& material)
{
	// デバイスの取得
	m_pRenderer = pDev;
	m_material = material;

	if (static_cast<int>(vertices.size()) == 0)
	{
		assert(0 && "頂点情報がありません。");
		return;
	}

	m_instanceCount = static_cast<UINT>(vertices.size() * 3);

	// 頂点バッファの作成
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeof(MeshVertex) * vertices.size();
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの作成
	auto hr = m_pRenderer->GetDev()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pVertexBuff.ReleaseAndGetAddressOf())
	);

	if (FAILED(hr))
	{
		assert(0 && "頂点バッファーの作成に失敗しました。");
		return;
	}

	// 頂点バッファのデータをビューに書き込む
	m_vbView.BufferLocation = m_pVertexBuff->GetGPUVirtualAddress();
	m_vbView.SizeInBytes = (UINT)resDesc.Width;		// 頂点バッファ全体のバイト数
	m_vbView.StrideInBytes = sizeof(MeshVertex);	// 1頂点あたりのバイト数

	resDesc.Width = sizeof(MeshFace) * faces.size();


	// インデックスバッファの作成
	hr = m_pRenderer->GetDev()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pIndexBuff.ReleaseAndGetAddressOf())
	);

	if (FAILED(hr))
	{
		assert(0 && "インデックスバッファーの作成に失敗しました。");
		return;
	}

	// インデックスバッファのデータをビューに書き込む
	m_ibView.BufferLocation = m_pIndexBuff->GetGPUVirtualAddress();
	m_ibView.SizeInBytes = (UINT)resDesc.Width;
	m_ibView.Format = DXGI_FORMAT_R32_UINT;

	// マップ
	MeshVertex* vbMap = nullptr;
	m_pVertexBuff->Map(0, nullptr, (void**)&vbMap);
	std::copy(begin(vertices), end(vertices), vbMap);
	m_pVertexBuff->Unmap(0, nullptr);

	MeshFace* ibMap = nullptr;
	m_pIndexBuff->Map(0, nullptr, (void**)&ibMap);
	std::copy(begin(faces), end(faces), ibMap);
	m_pIndexBuff->Unmap(0, nullptr);
}

void Mesh::DrawInstanced(UINT vertexCount) const
{
	m_pRenderer->GetCmdList()->IASetVertexBuffers(0, 1, &m_vbView);
	m_pRenderer->GetCmdList()->IASetIndexBuffer(&m_ibView);
	m_pRenderer->GetCmdList()->DrawIndexedInstanced(vertexCount, 1, 0, 0, 0);
}
