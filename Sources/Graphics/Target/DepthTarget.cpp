// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DepthTarget.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 17:00
// 概要   : 深度ターゲットクラスの実装
// 更新履歴
// 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "DepthTarget.h"
#include <Graphics/DescriptorPool/DescriptorPool.h>

// コンストラクタ
DepthTarget::DepthTarget()
	: m_pTarget(nullptr)
	, m_pHandleDSV(nullptr)
	, m_pHandleSRV(nullptr)
	, m_pPoolDSV(nullptr)
	, m_pPoolSRV(nullptr)
{
}

// デストラクタ
DepthTarget::~DepthTarget()
{
	Uninit();
}

// 初期化処理
bool DepthTarget::Init(DescriptorPool* pPoolRTV, DescriptorPool* pPoolSRV, uint32_t width, uint32_t height, DXGI_FORMAT format, float clearDepth, uint8_t clearStencil)
{
	auto pDevice = RENDERER.GetDevice();
	if (pDevice == nullptr || pPoolRTV == nullptr || width == 0 || height == 0)
	{
		return false;
	}

	assert(m_pHandleDSV == nullptr);
	assert(m_pPoolDSV == nullptr);

	m_pPoolDSV = pPoolRTV;
	m_pPoolDSV->AddRef();

	// ディスクリプタハンドルを取得
	m_pHandleDSV = m_pPoolDSV->AllocHandle();
	if (m_pHandleDSV == nullptr)
	{
		return false;
	}

	// シェーダーリソースビューが指定されている場合
	if (pPoolSRV != nullptr)
	{
		m_pPoolSRV = pPoolSRV;
		m_pPoolSRV->AddRef();

		m_pHandleSRV = m_pPoolSRV->AllocHandle();
		if (m_pHandleSRV == nullptr)
		{
			return false;
		}
	}

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソース設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = UINT64(width);
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	m_ClearDepth = clearDepth;
	m_ClearStencil = clearStencil;

	// クリア値
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.DepthStencil.Depth = m_ClearDepth;
	clearValue.DepthStencil.Stencil = m_ClearStencil;

	// リソースの生成
	auto hr = pDevice->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(m_pTarget.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// 深度ステンシルビューの設定
	m_DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_DSVDesc.Format = format;
	m_DSVDesc.Texture2D.MipSlice = 0;
	m_DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

	// 深度ステンシルビューの生成
	pDevice->CreateDepthStencilView(m_pTarget.Get(), &m_DSVDesc, m_pHandleDSV->HandleCPU);

	// シェーダーリソースビューが指定されている場合
	if (m_pHandleSRV != nullptr)
	{
		m_SRVDesc.Format = format;
		m_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		m_SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		m_SRVDesc.Texture2D.MipLevels = 1;
		m_SRVDesc.Texture2D.MostDetailedMip = 0;
		m_SRVDesc.Texture2D.PlaneSlice = 0;
		m_SRVDesc.Texture2D.ResourceMinLODClamp = 0;

		// シェーダーリソースビューの生成
		pDevice->CreateShaderResourceView(m_pTarget.Get(), &m_SRVDesc, m_pHandleSRV->HandleCPU);
	}

	return true;
}

// 終了処理
void DepthTarget::Uninit()
{
	m_pTarget.Reset();

	if (m_pPoolDSV != nullptr && m_pHandleDSV != nullptr)
	{
		m_pPoolDSV->FreeHandle(m_pHandleDSV);
		m_pHandleDSV = nullptr;
	}

	if (m_pPoolDSV != nullptr)
	{
		m_pPoolDSV->Release();
		m_pPoolDSV = nullptr;
	}

	if (m_pPoolSRV != nullptr && m_pHandleSRV != nullptr)
	{
		m_pPoolSRV->FreeHandle(m_pHandleSRV);
		m_pHandleSRV = nullptr;
	}

	if (m_pHandleSRV != nullptr)
	{
		m_pPoolSRV->Release();
		m_pPoolSRV = nullptr;
	}
}

void DepthTarget::ClearView(ID3D12GraphicsCommandList* pCmdList)
{
	pCmdList->ClearDepthStencilView(m_pHandleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, m_ClearDepth, m_ClearStencil, 0, nullptr);
}

// ディスクリプタハンドルを取得
DescriptorHandle* DepthTarget::GetHandleDSV() const
{
	return m_pHandleDSV;
}

// リソースを取得
ID3D12Resource* DepthTarget::GetResource() const
{
	return m_pTarget.Get();
}

// リソース設定を取得
D3D12_RESOURCE_DESC DepthTarget::GetDesc() const
{
	if (!m_pTarget)
	{
		return D3D12_RESOURCE_DESC();
	}

	return m_pTarget->GetDesc();
}

D3D12_DEPTH_STENCIL_VIEW_DESC DepthTarget::GetDSVDesc() const
{
	return m_DSVDesc;
}

D3D12_SHADER_RESOURCE_VIEW_DESC DepthTarget::GetSRVDesc() const
{
	return m_SRVDesc;
}
