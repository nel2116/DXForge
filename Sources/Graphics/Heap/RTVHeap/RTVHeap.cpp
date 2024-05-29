// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 12:00
// �T�v   : RTV�q�[�v�N���X�̎���
// �X�V����
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "RTVHeap.h"

int RTVHeap::CreateRTV(ID3D12Resource* pBuffer)
{
	if (m_useCount < m_nextRegisterNumber)
	{
		assert(0 && "�m�ۍς݂̃q�[�v�̈�𒴂��܂����B");
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_nextRegisterNumber * m_incrementSize;
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_pRenderer->GetDev()->CreateRenderTargetView(pBuffer, &desc, handle);
	return m_nextRegisterNumber++;
}
