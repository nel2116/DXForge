// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/10
// �쐬�� : �c���~�m��
// �T�v   : �e�N�X�`���N���X�̒�`
// �X�V���� : 2024/06/10 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Texture.h"
#include <Graphics/DescriptorPool/DescriptorPool.h>
#include <System/FileUtil.h>
#include <System/Logger.h>

Texture::Texture()
{
}

Texture::~Texture()
{
	Uninit();
}

bool Texture::Init(DescriptorPool* pPool, const wchar_t* filename)
{
	auto pDevice = RENDERER.GetDevice();

	if (pDevice == nullptr || pPool == nullptr || filename == nullptr)
	{
		assert(0 && "[Texture.cpp]�p�����[�^���s���ł��B");
		return false;
	}

	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	// �f�B�X�N���v�^�v�[����ݒ�
	m_pPool = pPool;
	m_pPool->AddRef();

	// �f�B�X�N���v�^�n���h�����擾
	m_pHandle = m_pPool->AllocHandle();
	if (!m_pHandle)
	{
		assert(0 && "[Texture.cpp]�f�B�X�N���v�^�n���h���̎擾�Ɏ��s���܂����B");
		return false;
	}

	// �t�@�C������e�N�X�`���𐶐�
	bool isCube = false;
	if (!CreateTexture(filename))
	{
		assert(0 && "[Texture.cpp]�e�N�X�`���̐����Ɏ��s���܂����B");
		return false;
	}

	// �V�F�[�_���\�[�X�r���[�̐ݒ�����߂�
	auto viewDesc = GetViewDesc(isCube);

	// �V�F�[�_���\�[�X�r���[�𐶐�
	pDevice->CreateShaderResourceView(m_pTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	// ����I��
	return true;
}

bool Texture::Init(DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isCube)
{
	auto pDevice = RENDERER.GetDevice();
	if (pDevice == nullptr || pPool == nullptr || pDesc == nullptr)
	{
		return false;
	}

	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	// �f�B�X�N���v�^�v�[����ݒ�.
	m_pPool = pPool;
	m_pPool->AddRef();

	// �f�B�X�N���v�^�n���h�����擾.
	m_pHandle = pPool->AllocHandle();
	if (m_pHandle == nullptr)
	{
		return false;
	}

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
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

	// �V�F�[�_���\�[�X�r���[�̐ݒ�����߂�.
	auto viewDesc = GetViewDesc(isCube);

	// �V�F�[�_���\�[�X�r���[�𐶐����܂�.
	pDevice->CreateShaderResourceView(m_pTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	return true;
}

void Texture::Uninit()
{
	// �f�B�X�N���v�^�n���h�������.
	if (m_pHandle != nullptr && m_pPool != nullptr)
	{
		m_pPool->FreeHandle(m_pHandle);
		m_pHandle = nullptr;
	}

	// �f�B�X�N���v�^�v�[�������.
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

bool Texture::CreateTexture(const wchar_t* filename)
{
	// �t�@�C���p�X������
	std::wstring texPath;
	if (!SearchFilePath(filename, texPath))
	{
		assert(0 && "[Texture.cpp]�e�N�X�`���t�@�C����������܂���ł����B");
		return false;
	}

	// �e�N�X�`���̐���
	{
		// �e�N�X�`���̓ǂݍ���
		DirectX::TexMetadata metadata = {};
		DirectX::ScratchImage scratch = {};
		auto hr = DirectX::LoadFromWICFile(texPath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratch);
		if (FAILED(hr))
		{
			ELOG("Error : DirectX::LoadFromWICFile() Failed. retcode = 0x%x", hr);
			return false;
		}

		// �e�N�X�`���̐ݒ�
		auto img = scratch.GetImage(0, 0, 0);

		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;						// D3D12_HEAP_TYPE_DEFAULT : GPU����̓ǂݏ���������
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPU�y�[�W�v���p�e�B�𖢎w��
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : �������v�[�����w��
		heapProp.CreationNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��
		heapProp.VisibleNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		// D3D12_RESOURCE_DIMENSION_TEXTURE2D : ���\�[�X��2�����e�N�X�`���ł��邱�Ƃ�����
		resDesc.Alignment = 0;										// 0 : ���\�[�X�̃A���C�����g���w�肵�Ȃ�
		resDesc.Width = metadata.width;								// �e�N�X�`���̉���
		resDesc.Height = metadata.height;							// �e�N�X�`���̏c��
		resDesc.DepthOrArraySize = 1;								// 1 : �e�N�X�`���̐[�x�܂��͔z��T�C�Y
		resDesc.MipLevels = 1;										// 1 : �~�b�v�}�b�v���x����
		resDesc.Format = metadata.format;							// �e�N�X�`���̃t�H�[�}�b�g
		resDesc.SampleDesc.Count = 1;								// 1 : �T���v�����O��
		resDesc.SampleDesc.Quality = 0;								// 0 : �T���v�����O�i��
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;				// D3D12_TEXTURE_LAYOUT_UNKNOWN : �e�N�X�`���f�[�^�̃��C�A�E�g�𖢎w��
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;					// D3D12_RESOURCE_FLAG_NONE : �I�v�V��������Ɏw�肵�Ȃ�

		// ���\�[�X�̐���
		hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : �q�[�v�̃I�v�V��������Ɏw�肵�Ȃ�
			&resDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_pTex.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
			return false;
		}


		// �e�N�X�`���̃A�b�v���[�h
		D3D12_HEAP_PROPERTIES heapPropUpload = {};
		heapPropUpload.Type = D3D12_HEAP_TYPE_CUSTOM;							// D3D12_HEAP_TYPE_UPLOAD : CPU����GPU�ւ̓]��������
		heapPropUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// D3D12_CPU_PAGE_PROPERTY_WRITE_BACK : CPU�y�[�W�v���p�e�B�������߂��\�ɐݒ�
		heapPropUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;				// D3D12_MEMORY_POOL_L0 : �������v�[����L0�ɐݒ�
		heapPropUpload.CreationNodeMask = 0;									// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��0���w��
		heapPropUpload.VisibleNodeMask = 0;										// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��0���w��

		D3D12_RESOURCE_DESC resDescUpload = {};
		resDescUpload.Format = metadata.format;									// �e�N�X�`���̃t�H�[�}�b�g
		resDescUpload.Width = metadata.width;									// �e�N�X�`���̉���
		resDescUpload.Height = metadata.height;									// �e�N�X�`���̏c��
		resDescUpload.DepthOrArraySize = metadata.arraySize;
		resDescUpload.SampleDesc.Count = 1;
		resDescUpload.SampleDesc.Quality = 0;
		resDescUpload.MipLevels = metadata.mipLevels;
		resDescUpload.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);	// D3D12_RESOURCE_DIMENSION_BUFFER : ���\�[�X���o�b�t�@�ł��邱�Ƃ�����
		resDescUpload.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDescUpload.Flags = D3D12_RESOURCE_FLAG_NONE;									// D3D12_RESOURCE_FLAG_NONE : �I�v�V��������Ɏw�肵�Ȃ�

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

		// �e�N�X�`���f�[�^�̃R�s�[
		hr = m_pTex->WriteToSubresource(0, nullptr, img->pixels, img->rowPitch, img->slicePitch);
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Resource::WriteToSubresource() Failed. retcode = 0x%x", hr);
			return false;
		}
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
		assert(0 && "[Texture.cpp]�o�b�t�@�͖��Ή��ł��B");
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
		// �z��O
		abort();    // �A�v�����~�߂�.
	}
	break;
	}
	return viewDesc;
}
