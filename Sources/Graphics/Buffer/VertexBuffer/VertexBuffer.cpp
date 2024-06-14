// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [VertexBuffer.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 11:30
// 概要   : 頂点バッファクラスの実装
// 更新履歴
// 2024/06/14 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "VertexBuffer.h"
#include <System/Logger.h>

// コンストラクタ
VertexBuffer::VertexBuffer()
	: m_pVB(nullptr)
{
	memset(&m_View, 0, sizeof(m_View));
}

// デストラクタ
VertexBuffer::~VertexBuffer()
{
	Uninit();
}

// 初期化処理
bool VertexBuffer::Init(size_t size, size_t stride, const void* pInitData)
{
	auto pDevice = RENDERER.GetDevice();
	// 引数チェック
	if (pDevice == nullptr || size == 0 || stride == 0)
	{
		return false;
	}

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = UINT64(size);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースを生成
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pVB.GetAddressOf()));
	if (FAILED(hr))
	{
		ELOG("[VertexBuffer.cpp]Error : Line69 : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
		return false;
	}

	// 頂点バッファビューの設定
	m_View.BufferLocation = m_pVB->GetGPUVirtualAddress();
	m_View.StrideInBytes = UINT(stride);
	m_View.SizeInBytes = UINT(size);

	// 初期化データがあれば，書き込んでおく
	if (pInitData != nullptr)
	{
		void* ptr = Map();
		if (ptr == nullptr)
		{
			return false;
		}

		memcpy(ptr, pInitData, size);

		m_pVB->Unmap(0, nullptr);
	}

	// 正常終了.
	return true;
}

// 終了処理
void VertexBuffer::Uninit()
{
	m_pVB.Reset();
	memset(&m_View, 0, sizeof(m_View));
}

// メモリマッピング
void* VertexBuffer::Map()
{
	void* ptr;
	auto hr = m_pVB->Map(0, nullptr, &ptr);
	if (FAILED(hr))
	{
		ELOG("[VertexBuffer.cpp]Error : Line110 : ID3D12Resource::Map() Failed. retcode = 0x%x", hr);
		return nullptr;
	}

	return ptr;
}

// メモリマッピングを解除
void VertexBuffer::Unmap()
{
	m_pVB->Unmap(0, nullptr);
}

// 頂点バッファビューを取得
D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetView() const
{
	return m_View;
}

