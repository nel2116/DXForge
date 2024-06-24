// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/10
// 作成者 : 田中ミノル
// 概要   : テクスチャクラスの定義
// 更新履歴 : 2024/06/10 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Texture.h"
#include <Graphics/DescriptorPool/DescriptorPool.h>
#include <System/FileUtil.h>
#include <System/Logger.h>
#include <System/DDSTextureLoader12.h>
#include <System/d3dx12.h>

namespace
{
	// テクスチャフォーマットをSRGBに変換
	DXGI_FORMAT ConvertToSRGB(DXGI_FORMAT format)
	{
		DXGI_FORMAT result = format;
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		{ result = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC1_UNORM:
		{ result = DXGI_FORMAT_BC1_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC2_UNORM:
		{ result = DXGI_FORMAT_BC2_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC3_UNORM:
		{ result = DXGI_FORMAT_BC3_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
		{ result = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
		{ result = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC7_UNORM:
		{ result = DXGI_FORMAT_BC7_UNORM_SRGB; }
		break;

		default:
			break;
		}

		return result;
	}
} // namespace 

Texture::Texture()
	: m_pTex(nullptr)
	, m_pHandle(nullptr)
	, m_pPool(nullptr)
{
}

Texture::~Texture()
{
	Uninit();
}

bool Texture::Init(DescriptorPool* pPool, const wchar_t* filename, bool isSRGB)
{
	auto pDevice = RENDERER.GetDevice();

	if (pDevice == nullptr || pPool == nullptr || filename == nullptr)
	{
		assert(0 && "[Texture.cpp]パラメータが不正です。");
		return false;
	}

	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	// ディスクリプタプールを設定
	m_pPool = pPool;
	m_pPool->AddRef();

	// ディスクリプタハンドルを取得
	m_pHandle = m_pPool->AllocHandle();
	if (!m_pHandle)
	{
		assert(0 && "[Texture.cpp]ディスクリプタハンドルの取得に失敗しました。");
		return false;
	}

	// ファイルからテクスチャを生成
	// DDSファイルの場合
	if (wcsstr(filename, L".dds") != nullptr || wcsstr(filename, L".DDS") != nullptr)
	{
		if (!CreateDDSTexture(filename, isSRGB))
		{
			assert(0 && "[Texture.cpp]CreateDDSTexture() Failed.");
			return false;
		}
	}
	// それ以外の場合
	else
	{
		if (!CreateTexture(filename, isSRGB))
		{
			assert(0 && "[Texture.cpp]CreateTexture() Failed.");
			return false;
		}
	}

	bool isCube = false;

	// シェーダリソースビューの設定を求める
	auto viewDesc = GetViewDesc(isCube);

	// シェーダリソースビューを生成
	pDevice->CreateShaderResourceView(m_pTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	// 正常終了
	return true;
}

bool Texture::Init(DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isSRGB, bool isCube)
{
	auto pDevice = RENDERER.GetDevice();
	if (pDevice == nullptr || pPool == nullptr || pDesc == nullptr)
	{
		return false;
	}

	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	// ディスクリプタプールを設定
	m_pPool = pPool;
	m_pPool->AddRef();

	// ディスクリプタハンドルを取得
	m_pHandle = pPool->AllocHandle();
	if (m_pHandle == nullptr)
	{
		return false;
	}

	D3D12_RESOURCE_DESC desc = *pDesc;
	if (isSRGB)
	{
		desc.Format = ConvertToSRGB(desc.Format);
	}

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 0;
	prop.VisibleNodeMask = 0;

	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		pDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pTex.GetAddressOf())
	);
	if (FAILED(hr))
	{
		ELOG("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
		return false;
	}

	vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);	// すべて255で埋める

	// データ転送
	hr = m_pTex->WriteToSubresource(0, nullptr, data.data(), 4 * 4, static_cast<UINT>(data.size()));
	if (FAILED(hr))
	{
		ELOG("Error : WriteToSubresource() Failed. retcode = 0x%x", hr);
		return false;
	}

	// シェーダリソースビューの設定を求める
	auto viewDesc = GetViewDesc(isCube);

	// シェーダリソースビューを生成します
	pDevice->CreateShaderResourceView(m_pTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	return true;
}

void Texture::Uninit()
{
	// ディスクリプタハンドルを解放.
	if (m_pHandle != nullptr && m_pPool != nullptr)
	{
		m_pPool->FreeHandle(m_pHandle);
		m_pHandle = nullptr;
	}

	// ディスクリプタプールを解放.
	if (m_pPool != nullptr)
	{
		m_pPool->Release();
		m_pPool = nullptr;
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetHandleCPU() const
{
	if (m_pHandle != nullptr)
	{
		return m_pHandle->HandleCPU;
	}

	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetHandleGPU() const
{
	if (m_pHandle != nullptr)
	{
		return m_pHandle->HandleGPU;
	}

	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

bool Texture::CreateTexture(const wchar_t* filename, bool isSRGB)
{
	// ファイルパスを検索
	std::wstring texPath;
	if (!SearchFilePathW(filename, texPath))
	{
		assert(0 && "[Texture.cpp]テクスチャファイルが見つかりませんでした。");
		return false;
	}

	// テクスチャの生成
	{
		// テクスチャの読み込み
		DirectX::TexMetadata metadata = {};
		DirectX::ScratchImage scratch = {};
		DirectX::WIC_FLAGS flag = isSRGB ? DirectX::WIC_FLAGS_FORCE_SRGB : DirectX::WIC_FLAGS_NONE;
		auto hr = DirectX::LoadFromWICFile(texPath.c_str(), flag, &metadata, scratch);
		if (FAILED(hr))
		{
			ELOG("Error : DirectX::LoadFromWICFile() Failed. retcode = 0x%x", hr);
			return false;
		}

		// テクスチャの設定
		auto img = scratch.GetImage(0, 0, 0);

		// ヒーププロパティ
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;						// D3D12_HEAP_TYPE_DEFAULT : GPUからの読み書きが高速
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPUページプロパティを未指定
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : メモリプール未指定
		heapProp.CreationNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定
		heapProp.VisibleNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定

		// リソースの設定
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		// D3D12_RESOURCE_DIMENSION_TEXTURE2D : リソースが2次元テクスチャであることを示す
		resDesc.Alignment = 0;										// 0 : リソースのアライメントを指定しない
		resDesc.Width = static_cast<UINT>(metadata.width);			// テクスチャの横幅
		resDesc.Height = static_cast<UINT>(metadata.height);		// テクスチャの縦幅
		resDesc.DepthOrArraySize = 1;								// 1 : テクスチャの深度または配列サイズ
		resDesc.MipLevels = 1;										// 1 : ミップマップレベル数
		resDesc.Format = metadata.format;							// テクスチャのフォーマット
		resDesc.SampleDesc.Count = 1;								// 1 : サンプリング数
		resDesc.SampleDesc.Quality = 0;								// 0 : サンプリング品質
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;				// D3D12_TEXTURE_LAYOUT_UNKNOWN : テクスチャデータのレイアウトを未指定
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// リソースの生成
		hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : ヒープのオプションを特に指定しない
			&resDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_pTex.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
			return false;
		}


		// テクスチャのアップロード
		D3D12_HEAP_PROPERTIES heapPropUpload = {};
		heapPropUpload.Type = D3D12_HEAP_TYPE_CUSTOM;							// D3D12_HEAP_TYPE_UPLOAD : CPUからGPUへの転送が高速
		heapPropUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// D3D12_CPU_PAGE_PROPERTY_WRITE_BACK : CPUページプロパティを書き戻し可能に設定
		heapPropUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;				// D3D12_MEMORY_POOL_L0 : メモリプールをL0に設定
		heapPropUpload.CreationNodeMask = 0;									// 単一GPUによる動作を前提としているため0を指定
		heapPropUpload.VisibleNodeMask = 0;										// 単一GPUによる動作を前提としているため0を指定

		D3D12_RESOURCE_DESC resDescUpload = {};
		resDescUpload.Format = metadata.format;									// テクスチャのフォーマット
		resDescUpload.Width = static_cast<UINT>(metadata.width);				// テクスチャの横幅
		resDescUpload.Height = static_cast<UINT>(metadata.height);				// テクスチャの縦幅
		resDescUpload.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
		resDescUpload.SampleDesc.Count = 1;
		resDescUpload.SampleDesc.Quality = 0;
		resDescUpload.MipLevels = static_cast<UINT16>(metadata.mipLevels);
		resDescUpload.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);	// D3D12_RESOURCE_DIMENSION_BUFFER : リソースがバッファであることを示す
		resDescUpload.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDescUpload.Flags = D3D12_RESOURCE_FLAG_NONE;									// D3D12_RESOURCE_FLAG_NONE : オプションを特に指定しない

		hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapPropUpload,
			D3D12_HEAP_FLAG_NONE,
			&resDescUpload,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(m_pTex.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
			return false;
		}

		// テクスチャデータのコピー
		hr = m_pTex->WriteToSubresource(0, nullptr, img->pixels, static_cast<UINT>(img->rowPitch), static_cast<UINT>(img->slicePitch));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Resource::WriteToSubresource() Failed. retcode = 0x%x", hr);
			return false;
		}
	}
	return true;
}

bool Texture::CreateDDSTexture(const wchar_t* filename, bool isSRGB)
{
	// ファイルパスを検索
	std::wstring texPath;
	if (!SearchFilePathW(filename, texPath))
	{
		assert(0 && "[Texture.cpp]テクスチャファイルが見つかりませんでした。");
		return false;
	}

	// テクスチャの生成
	{
		// テクスチャの読み込み
		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		unsigned int loadFlags = isSRGB ? DirectX::DDS_LOADER_FORCE_SRGB : DirectX::DDS_LOADER_DEFAULT;
		HRESULT hr = DirectX::LoadDDSTextureFromFileEx(
			RENDERER.GetDevice(),                // Direct3Dデバイス
			texPath.c_str(),                     // テクスチャファイルのパス
			0,                                   // 最大サイズ（省略可能）
			D3D12_RESOURCE_FLAG_NONE,            // リソースフラグ
			loadFlags,                           // ロードフラグ
			m_pTex.GetAddressOf(),               // テクスチャリソース
			ddsData,                             // DDSデータ
			subresources                         // サブリソースデータ
		);
		if (FAILED(hr))
		{
			ELOG("Error : DirectX::LoadDDSTextureFromFileEx() Failed. retcode = 0x%x", hr);
			return false;
		}

		// テクスチャの設定
		D3D12_RESOURCE_DESC resDesc = m_pTex->GetDesc();

		const UINT subresourceSize = static_cast<UINT>(subresources.size());
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pTex.Get(), 0, subresourceSize);

		// アップロードバッファの生成
		Microsoft::WRL::ComPtr<ID3D12Resource> texture;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto heapDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		// リソースの設定
		hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&heapDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(texture.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
			return false;
		}

		// コマンドリストのリセット
		auto pCmdList = RENDERER.GetCmdList()->Reset();

		// コマンドリストにコピーコマンドを記録
		UpdateSubresources(
			pCmdList,
			m_pTex.Get(),
			texture.Get(),
			0, 0, subresourceSize,
			&subresources[0]);

		// テクスチャリソースの状態をコピー先からピクセルシェーダリソースに変更
		RENDERER.TransitionResource(m_pTex.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// コマンドリストの実行
		pCmdList->Close();
		ID3D12CommandList* cmdLists[] = { pCmdList };
		RENDERER.GetCmdQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		// コマンドリストの待機
		RENDERER.WaitGpu();
	}
	return true;
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture::GetViewDesc(bool isCube)
{
	auto desc = m_pTex->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

	viewDesc.Format = desc.Format;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_BUFFER:
	{
		assert(0 && "[Texture.cpp]バッファは未対応です。");
		break;
	}
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	{
		if (desc.DepthOrArraySize > 1)
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

			viewDesc.Texture1DArray.MostDetailedMip = 0;
			viewDesc.Texture1DArray.MipLevels = desc.MipLevels;
			viewDesc.Texture1DArray.FirstArraySlice = 0;
			viewDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
			viewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

			viewDesc.Texture1D.MostDetailedMip = 0;
			viewDesc.Texture1D.MipLevels = desc.MipLevels;
			viewDesc.Texture1D.ResourceMinLODClamp = 0.0f;
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		if (isCube)
		{
			if (desc.DepthOrArraySize > 6)
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;

				viewDesc.TextureCubeArray.MostDetailedMip = 0;
				viewDesc.TextureCubeArray.MipLevels = desc.MipLevels;
				viewDesc.TextureCubeArray.First2DArrayFace = 0;
				viewDesc.TextureCubeArray.NumCubes = (desc.DepthOrArraySize / 6);
				viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

				viewDesc.TextureCube.MostDetailedMip = 0;
				viewDesc.TextureCube.MipLevels = desc.MipLevels;
				viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
		}
		else
		{
			if (desc.DepthOrArraySize > 1)
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;

					viewDesc.Texture2DMSArray.FirstArraySlice = 0;
					viewDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

					viewDesc.Texture2DArray.MostDetailedMip = 0;
					viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
					viewDesc.Texture2DArray.FirstArraySlice = 0;
					viewDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
					viewDesc.Texture2DArray.PlaneSlice = 0;
					viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
				}
			}
			else
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

					viewDesc.Texture2D.MostDetailedMip = 0;
					viewDesc.Texture2D.MipLevels = desc.MipLevels;
					viewDesc.Texture2D.PlaneSlice = 0;
					viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				}
			}
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
	{
		viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

		viewDesc.Texture3D.MostDetailedMip = 0;
		viewDesc.Texture3D.MipLevels = desc.MipLevels;
		viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
	}
	break;

	default:
	{
		// 想定外
		abort();    // アプリを止める.
	}
	break;
	}
	return viewDesc;
}
