#include "stdafx.h"
#include "Polygon.h"

bool Polygon::Init()
{
	// ���_�o�b�t�@�̐���
	{
		// ���_�f�[�^
		Vertex vertices[] =
		{
			{DirectX::XMFLOAT3(-1.0f,-1.0f,0.0f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
			{ DirectX::XMFLOAT3(1.0f,-1.0f,0.0f),DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
			{ DirectX::XMFLOAT3(0.0f, 1.0f,0.0f),DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		};

		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = sizeof(vertices);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// ���\�[�X�̐���
		auto hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_pVB.ReleaseAndGetAddressOf()));

		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]���_�o�b�t�@�̐����Ɏ��s���܂����B");
			return false;
		}

		// �}�b�s���O
		void* p = nullptr;
		hr = m_pVB->Map(0, nullptr, &p);
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]���_�o�b�t�@�̃}�b�s���O�Ɏ��s���܂����B");
			return false;
		}

		// ���_�f�[�^���}�b�s���O��ɐݒ�
		memcpy(p, vertices, sizeof(vertices));

		// �}�b�v���O����
		m_pVB->Unmap(0, nullptr);

		// ���_�o�b�t�@�r���[�̐ݒ�
		m_vbView.BufferLocation = m_pVB->GetGPUVirtualAddress();
		m_vbView.SizeInBytes = static_cast<UINT>(sizeof(vertices));
		m_vbView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));
	}

	// �萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�̐���
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 2;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;

		auto hr = RENDERER.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapCBV.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]�萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
			return false;
		}
	}

	// �萔�o�b�t�@�̐���
	{
		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = sizeof(Transform);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto incrementSize = RENDERER.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < 2; ++i)
		{
			// ���\�[�X�̐���
			auto hr = RENDERER.GetDevice()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_pCB[i].ReleaseAndGetAddressOf()));

			if (FAILED(hr))
			{
				assert(0 && "[Polygon.cpp]�萔�o�b�t�@�̐����Ɏ��s���܂����B");
				return false;
			}

			auto address = m_pCB[i]->GetGPUVirtualAddress();
			auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV->GetGPUDescriptorHandleForHeapStart();

			handleCPU.ptr += incrementSize * i;
			handleGPU.ptr += incrementSize * i;

			// �萔�o�b�t�@�r���[�̐ݒ�
			m_CBV[i].HandleCPU = handleCPU;
			m_CBV[i].HandleGPU = handleGPU;
			m_CBV[i].Desc.BufferLocation = address;
			m_CBV[i].Desc.SizeInBytes = sizeof(Transform);

			// �萔�o�b�t�@�[�r���[�̐���
			RENDERER.GetDevice()->CreateConstantBufferView(&m_CBV[i].Desc, m_CBV[i].HandleCPU);

			// �}�b�s���O
			hr = m_pCB[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBV[i].pBuffer));
			if (FAILED(hr))
			{
				assert(0 && "[Polygon.cpp]�萔�o�b�t�@�̃}�b�s���O�Ɏ��s���܂����B");
				return false;
			}

			auto eye = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
			auto focus = DirectX::XMVectorZero();
			auto up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);

			// �ϊ��s��̐ݒ�

		}
	}
}

void Polygon::Draw()
{
}
