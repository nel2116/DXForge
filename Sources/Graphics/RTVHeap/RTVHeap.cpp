// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 16:30
// �T�v   : RTVHeap�N���X�̎���
// �X�V����
// 2021/05/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "RTVHeap.h"

// ====== �����o�֐� ======
bool RTVHeap::Create(ID3D12Device* pDevice, UINT useCount)
{
	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// �����_�[�^�[�Q�b�g�Ƃ����g��
	desc.NodeMask = 0;
	desc.NumDescriptors = useCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �t���O�͓��ɂȂ�

	auto hr = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeap));
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		MessageBox(nullptr, "RTVHeap�̐����Ɏ��s���܂���", "RTVHeap Error", MB_OK);
		return false;
	}

	m_useCount = useCount;
	m_incrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_pDevice = pDevice;

	return true;
}

int RTVHeap::CreateRTV(ID3D12Resource* pBuffer)
{
	if (m_useCount < m_nextIndex)
	{
		// �G���[���b�Z�[�W
		MessageBox(nullptr, "�m�ۍς݂̃q�[�v�̈�𒴂��܂����B", "RTVHeap Error", MB_OK);
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_nextIndex * m_incrementSize;
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_pDevice->CreateRenderTargetView(pBuffer, &desc, handle);
	return m_nextIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVHeap::GetRTVCPUHandle(int number)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_incrementSize * number;
	return handle;
}
