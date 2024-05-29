#include "stdafx.h"
#include "CBVSRVUAVHeap.h"

int CBVSRVUAVHeap::CreateSRV(ID3D12Resource* pBuffer)
{
	if (m_useCount.y < m_nextRegisterNumber)
	{
		assert(0 && "確保済みのヒープ領域を超えました。");
		return -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += ((UINT64)m_useCount.x + 1) * m_incrementSize + (UINT64)m_nextRegisterNumber * m_incrementSize;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = pBuffer->GetDesc().Format;

	if (pBuffer->GetDesc().Format == DXGI_FORMAT_R32_TYPELESS)
	{
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	}

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	m_pRenderer->GetDev()->CreateShaderResourceView(pBuffer, &srvDesc, handle);

	return m_nextRegisterNumber++;
}

const D3D12_GPU_DESCRIPTOR_HANDLE CBVSRVUAVHeap::GetGPUHandle(int number)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += (UINT64)m_incrementSize * ((UINT64)m_useCount.x + 1);
	handle.ptr += (UINT64)m_incrementSize * number;
	return handle;
}

void CBVSRVUAVHeap::SetHeap()
{
	ID3D12DescriptorHeap* ppHeaps[] = { m_pHeap.Get() };
	m_pRenderer->GetCmdList()->SetDescriptorHeaps(1, ppHeaps);
}
