#include "stdafx.h"
#include "Texture.h"

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

	hr = pDev->GetDev()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_pTexture.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(0 && "テクスチャバッファの作成に失敗しました。");
		return false;
	}

	hr = m_pTexture->WriteToSubresource(0, nullptr, pImage->pixels, (UINT)pImage->rowPitch, (UINT)pImage->slicePitch);
	if (FAILED(hr))
	{
		assert(0 && "テクスチャデータの書き込みに失敗しました。");
		return false;
	}
	m_srvNumber = pDev->GetCBVSRVUAVHeap()->CreateSRV(m_pTexture.Get());

	return true;
}

void Texture::Set(int index)
{
	m_pRenderer->GetCmdList()->SetGraphicsRootDescriptorTable(index, m_pRenderer->GetCBVSRVUAVHeap()->GetGPUHandle(m_srvNumber));
}
