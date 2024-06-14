// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/13 17:50
// �쐬�� : �c���~�m��
// �T�v   : �f�B�X�N���v�^�v�[���N���X�̎���
// �X�V����
// 2024/06/13 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "DescriptorPool.h"

DescriptorPool::DescriptorPool()
	: m_RefCount(1)
	, m_pHeap(nullptr)
	, m_Pool()
	, m_DescriptorSize(0)
{
}

DescriptorPool::~DescriptorPool()
{
	m_Pool.Uninit();
	m_pHeap.Reset();
	m_DescriptorSize = 0;
}

void DescriptorPool::AddRef()
{
	m_RefCount++;
}

void DescriptorPool::Release()
{
	m_RefCount--;
	if (m_RefCount == 0)
	{
		delete this;
	}
}

uint32_t DescriptorPool::GetCount() const
{
	return m_RefCount;
}

DescriptorHandle* DescriptorPool::AllocHandle()
{
	// �������֐�
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
	{
		auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();

		handleCPU.ptr += m_DescriptorSize * index;
		handleGPU.ptr += m_DescriptorSize * index;

		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};

	// �������֐������s���Ă���n���h����Ԃ�
	return m_Pool.Alloc(func);
}

void DescriptorPool::FreeHandle(DescriptorHandle*& pHandle)
{
	if (!pHandle)
	{
		// �n���h�����v�[���ɖ߂�
		m_Pool.Free(pHandle);
		pHandle = nullptr;
	}
}

uint32_t DescriptorPool::GetAvailableHandleCount() const
{
	return m_Pool.GetAvailableCount();
}

uint32_t DescriptorPool::GetAllocatedHandleCount() const
{
	return m_Pool.GetUsedCount();
}

uint32_t DescriptorPool::GetHandleCount() const
{
	return m_Pool.GetSize();
}

ID3D12DescriptorHeap* const DescriptorPool::GetHeap() const
{
	return m_pHeap.Get();
}

bool DescriptorPool::Create(const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, DescriptorPool** ppPool)
{
	auto pDevice = RENDERER.GetDevice();
	// �����`�F�b�N
	if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr)
	{
		return false;
	}

	// �C���X�^���X�𐶐�
	auto instance = NEW DescriptorPool();
	// �C���X�^���X�������ł��Ȃ�����
	if (!instance)
	{
		return false;
	}

	// �f�B�X�N���v�^�q�[�v�̐���
	auto hr = pDevice->CreateDescriptorHeap(pDesc, IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));

	// ���s������J���������s���Ă���I��
	if (FAILED(hr))
	{
		instance->Release();
		return false;
	}

	// �v�[�������������܂��B
	if (!instance->m_Pool.Init(pDesc->NumDescriptors))
	{
		instance->Release();
		return false;
	}

	// �f�B�X�N���v�^�T�C�Y���擾
	instance->m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

	// �C���X�^���X��ݒ�
	*ppPool = instance;

	// ����I��
	return true;
}