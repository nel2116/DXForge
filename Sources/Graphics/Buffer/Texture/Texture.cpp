// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Texture.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 12:00
// 概要   : テクスチャクラスの実装
// 更新履歴
// 2024/05/29 作成
// 2024/05/30 Bufferクラスを継承するように変更
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Texture.h"

// ====== メンバ変数の初期化 ======
Microsoft::WRL::ComPtr<ID3D12Resource> Texture::m_pVBuffer = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> Texture::m_pIBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Texture::m_vbView = {};
D3D12_INDEX_BUFFER_VIEW Texture::m_ibView = {};
std::vector<Vertex> Texture::m_vertex;
std::vector<UINT> Texture::m_index;

// ====== メンバ関数 ======

bool Texture::Init()
{
	m_vertex.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f));	// 左下
	m_vertex.emplace_back(DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f));		// 左上
	m_vertex.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));		// 右下
	m_vertex.emplace_back(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f));		// 右上

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeof(Vertex) * m_vertex.size();
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto hr = Renderer::Instance().GetDev()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pVBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(hr)) { assert(0 && "テクスチャ用の頂点バッファーの作成に失敗しました。"); return false; }

	m_vbView.BufferLocation = m_pVBuffer->GetGPUVirtualAddress();
	m_vbView.SizeInBytes = (UINT)resDesc.Width;
	m_vbView.StrideInBytes = sizeof(Vertex);

	// インデックスデータ
	m_index.emplace_back(0);
	m_index.emplace_back(1);
	m_index.emplace_back(2);
	m_index.emplace_back(2);
	m_index.emplace_back(1);
	m_index.emplace_back(3);

	resDesc.Width = sizeof(UINT) * m_index.size();

	// インデックスバッファの作成
	hr = Renderer::Instance().GetDev()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pIBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(hr)) { assert(0 && "テクスチャ用のインデックスバッファーの作成に失敗しました。"); return false; }

	m_ibView.BufferLocation = m_pIBuffer->GetGPUVirtualAddress();
	m_ibView.SizeInBytes = (UINT)resDesc.Width;
	m_ibView.Format = DXGI_FORMAT_R32_UINT;

	// 頂点バッファに書き込む
	Vertex* vbMap = nullptr;
	{
		hr = m_pVBuffer->Map(0, nullptr, (void**)&vbMap);
		if (FAILED(hr)) { assert(0 && "テクスチャ用の頂点バッファのマップに失敗しました。"); return false; }
		std::copy(std::begin(m_vertex), std::end(m_vertex), vbMap);
		m_pVBuffer->Unmap(0, nullptr);
	}

	// インデックスバッファに書き込む
	UINT* ibMap = nullptr;
	{
		hr = m_pIBuffer->Map(0, nullptr, (void**)&ibMap);
		if (FAILED(hr)) { assert(0 && "テクスチャ用のインデックスバッファのマップに失敗しました。"); return false; }
		std::copy(std::begin(m_index), std::end(m_index), ibMap);
		m_pIBuffer->Unmap(0, nullptr);
	}

	return true;
}

bool Texture::Load(Renderer* pDev, const string& filePath)
{
	m_pRenderer = pDev;

	wchar_t wFilePath[128];
	MultiByteToWideChar(CP_ACP, 0, filePath.c_str(), -1, wFilePath, _countof(wFilePath));

	DirectX::TexMetadata metaData = {};
	DirectX::ScratchImage scratchImage = {};
	const DirectX::Image* pImage = nullptr;

	auto hr = DirectX::LoadFromWICFile(wFilePath, DirectX::WIC_FLAGS_NONE, &metaData, scratchImage);
	if (FAILED(hr))
	{
		assert(0 && "テクスチャの読み込みに失敗しました。");
		return false;
	}

	pImage = scratchImage.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	resDesc.Format = metaData.format;
	resDesc.Width = (UINT64)metaData.width;
	resDesc.Height = (UINT)metaData.height;
	resDesc.DepthOrArraySize = (UINT16)metaData.arraySize;
	resDesc.MipLevels = (UINT16)metaData.mipLevels;
	resDesc.SampleDesc.Count = 1;

	hr = pDev->GetDev()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_pBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(0 && "テクスチャバッファの作成に失敗しました。");
		return false;
	}

	hr = m_pBuffer->WriteToSubresource(0, nullptr, pImage->pixels, (UINT)pImage->rowPitch, (UINT)pImage->slicePitch);
	if (FAILED(hr))
	{
		assert(0 && "テクスチャデータの書き込みに失敗しました。");
		return false;
	}
	m_srvNumber = pDev->GetCBVSRVUAVHeap()->CreateSRV(m_pBuffer.Get());

	return true;
}

void Texture::Set(int index) const
{
	m_pRenderer->GetCmdList()->SetGraphicsRootDescriptorTable(index, m_pRenderer->GetCBVSRVUAVHeap()->GetGPUHandle(m_srvNumber));
}

void Texture::Draw() const
{
	m_pRenderer->GetCmdList()->IASetVertexBuffers(0, 1, &m_vbView);
	m_pRenderer->GetCmdList()->IASetIndexBuffer(&m_ibView);
	m_pRenderer->GetCmdList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
