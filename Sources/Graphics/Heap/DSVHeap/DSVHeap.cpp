// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DSVHeap.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/30 09:30
// �T�v   : �[�x�X�e���V���q�[�v�N���X�̎���
// �X�V����
// 2024/05/30 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "DSVHeap.h"

int DSVHeap::CreateDSV(ID3D12Resource* pBuffer, DXGI_FORMAT format)
{
	if (m_useCount < m_nextRegisterNumber)
	{
		assert(0 && "�m�ۍς݂̃q�[�v�̈�𒴂��܂���");
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_nextRegisterNumber * m_incrementSize;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_pRenderer->GetDev()->CreateDepthStencilView(pBuffer, &dsvDesc, handle);

	return m_nextRegisterNumber++;
}
