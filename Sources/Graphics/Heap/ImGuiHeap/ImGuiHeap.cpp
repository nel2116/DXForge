// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [imuGuiHeap.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/01 16:00
// �T�v   : imuGui�q�[�v�N���X�̎���
// �X�V����
// 2024/06/01 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "ImGuiHeap.h"

int ImGuiHeap::CreateimuGui(ID3D12Resource* pBuffer, DXGI_FORMAT format)
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
