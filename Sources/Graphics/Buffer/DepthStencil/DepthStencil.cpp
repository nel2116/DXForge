#include "stdafx.h"
#include "DepthStencil.h"

bool DepthStencil::Create(Renderer* pDev, const DirectX::XMFLOAT2& resolution, DepthStencilFormat format)
{
	m_pRenderer = pDev;

	// �f�v�X�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Width = static_cast<UINT>(resolution.x);
	resDesc.Height = static_cast<UINT>(resolution.y);
	resDesc.DepthOrArraySize = 1;
	resDesc.Format = static_cast<DXGI_FORMAT>(format);
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// �f�v�X�o�b�t�@�̃t�H�[�}�b�g�ƍő�[�x�l��ݒ�
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.DepthStencil.Depth = 1.0f;

	switch (format)
	{
	case DepthStencilFormat::DepthQuality:
		clearValue.Format = DXGI_FORMAT_D16_UNORM;
		break;
	case DepthStencilFormat::DepthHighQuality:
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	case DepthStencilFormat::D3pthHighQualityAndStencil:
		clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		break;
	}

	// �ݒ�����Ƀf�v�X�o�b�t�@�𐶐�
	auto hr = pDev->GetDev()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(m_pBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(0 && "�f�v�X�X�e���V���o�b�t�@�̐����Ɏ��s���܂����B");
		return false;
	}

	// �f�v�X�X�e���V���r���[�̍쐬
	m_dsvNumber = m_pRenderer->GetDSVHeap()->CreateDSV(m_pBuffer.Get(), clearValue.Format);

	return true;
}

void DepthStencil::ClearBuffer()
{
	// �f�v�X�o�b�t�@�̃N���A
	m_pRenderer->GetCmdList()->ClearDepthStencilView(m_pRenderer->GetDSVHeap()->GetCPUHandle(m_dsvNumber), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
