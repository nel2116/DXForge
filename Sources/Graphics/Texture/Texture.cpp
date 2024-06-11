// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/10
// �쐬�� : �c���~�m��
// �T�v   : �e�N�X�`���N���X�̒�`
// �X�V���� : 2024/06/10 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Texture.h"
#include <System/FileUtil.h>

bool Texture::Init()
{
	// ���_�o�b�t�@�̐���
	{
		// ���_�f�[�^
		Vertex vertices[] =
		{
			{DirectX::XMFLOAT3(-1.0f, 1.0f,0.0f),DirectX::XMFLOAT2(0.0f,0.0f)},
			{DirectX::XMFLOAT3(1.0f, 1.0f,0.0f),DirectX::XMFLOAT2(1.0f,0.0f)},
			{DirectX::XMFLOAT3(1.0f,-1.0f,0.0f),DirectX::XMFLOAT2(1.0f,1.0f)},
			{DirectX::XMFLOAT3(-1.0f,-1.0f,0.0f),DirectX::XMFLOAT2(0.0f,1.0f)},
		};

		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;						// D3D12_HEAP_TYPE_UPLOAD : CPU����GPU�ւ̓]��������
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPU�y�[�W�v���p�e�B�𖢎w��
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : �������v�[�����w��
		heapProp.CreationNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��
		heapProp.VisibleNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;		// D3D12_RESOURCE_DIMENSION_BUFFER : ���\�[�X���o�b�t�@�ł��邱�Ƃ�����
		resDesc.Alignment = 0;										// 0 : ���\�[�X�̃A���C�����g���w�肵�Ȃ�
		resDesc.Width = sizeof(vertices);							// ���_�f�[�^�̃T�C�Y
		resDesc.Height = 1;											// 1 : �o�b�t�@�̏ꍇ��1���w��A�e�N�X�`���̏ꍇ�͏c�����w��
		resDesc.DepthOrArraySize = 1;								// 1 : �o�b�t�@�̏ꍇ��1���w��A�e�N�X�`���̏ꍇ�͔z�񐔂��w��A�{�����[���e�N�X�`���̏ꍇ�͉��s�����w��
		resDesc.MipLevels = 1;										// 1 : �~�b�v�}�b�v���x�������w��
		resDesc.Format = DXGI_FORMAT_UNKNOWN;						// DXGI_FORMAT_UNKNOWN : �t�H�[�}�b�g���w��
		resDesc.SampleDesc.Count = 1;								// 1 : �T���v�����O�����w��
		resDesc.SampleDesc.Quality = 0;								// 0 : �T���v�����O�i�����w��
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;			// D3D12_TEXTURE_LAYOUT_ROW_MAJOR : �s�D��ŕۑ����ꂽ�e�N�X�`���f�[�^���w��
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;					// D3D12_RESOURCE_FLAG_NONE : �I�v�V��������Ɏw�肵�Ȃ�

		// ���\�[�X�̐���
		// ��1������HEAP_PROPERTIES�\���̂̃|�C���^
		// ��2������D3D12_HEAP_FLAG�񋓌^�̒l
		// ��3������D3D12_RESOURCE_DESC�\���̂̃|�C���^
		// ��4�����̓��\�[�X�̏�����Ԃ�ݒ�
		// ��5�����̓����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���e�N�X�`���̂��߂̃I�v�V����
		// ��6�����̓C���^�[�t�F�XID
		auto hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : �q�[�v�̃I�v�V��������Ɏw�肵�Ȃ�
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,							// D3D12_RESOURCE_DIMENSION_BUFFER���w�肵���ꍇ��nullptr���w��
			IID_PPV_ARGS(m_pVB.ReleaseAndGetAddressOf()));

		if (FAILED(hr))
		{	// ���s��
			assert(0 && "[Polygon.cpp]���_�o�b�t�@�̐����Ɏ��s���܂����B");
			return false;
		}

		// �}�b�s���O
		void* p = nullptr;
		// �������̓T�u���\�[�X�̃C���f�b�N�X
		// �������̓}�b�s���O�̊J�n�I�t�Z�b�g�ƏI���I�t�Z�b�g��ݒ�ł���Bnullptr���w�肷��ƑS�̂��}�b�s���O
		// ��O�����̓��\�[�X�f�[�^�ւ̃|�C���^���󂯎�郁�����u���b�N�ւ̃|�C���^
		hr = m_pVB->Map(0, nullptr, &p);
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]���_�o�b�t�@�̃}�b�s���O�Ɏ��s���܂����B");
			return false;
		}

		// ���_�f�[�^���}�b�s���O��ɐݒ�
		memcpy(p, vertices, sizeof(vertices));

		// �}�b�v���O����
		// �������̓T�u���\�[�X�̃C���f�b�N�X
		// ��������CPU���ύX�����͈͂��w��A����͑S�̂�ύX���Ă��邽��nullptr��ݒ�
		m_pVB->Unmap(0, nullptr);

		// ���_�o�b�t�@�r���[�̐ݒ�
		m_vbView.BufferLocation = m_pVB->GetGPUVirtualAddress();	// �o�b�t�@��GPU���z�A�h���X���擾
		m_vbView.SizeInBytes = static_cast<UINT>(sizeof(vertices));	// ���_�o�b�t�@�S�̂̃T�C�Y��ݒ�
		m_vbView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));	// 1���_�̃T�C�Y��ݒ�
	}

	// �C���f�b�N�X�o�b�t�@�̐���
	{
		uint32_t indices[] = { 0,1,2,0,2,3 };

		// �q�[�v�v���p�e�B
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;						// D3D12_HEAP_TYPE_UPLOAD : CPU����GPU�ւ̓]��������
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPU�y�[�W�v���p�e�B�𖢎w��
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : �������v�[�����w��
		heapProp.CreationNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��
		heapProp.VisibleNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��

		// ���\�[�X�̐ݒ�
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;		// D3D12_RESOURCE_DIMENSION_BUFFER : ���\�[�X���o�b�t�@�ł��邱�Ƃ�����
		resDesc.Alignment = 0;										// 0 : ���\�[�X�̃A���C�����g���w�肵�Ȃ�
		resDesc.Width = sizeof(indices);							// �C���f�b�N�X�f�[�^�̃T�C�Y
		resDesc.Height = 1;											// 1 : �o�b�t�@�̏ꍇ��1���w��A�e�N�X�`���̏ꍇ�͏c�����w��
		resDesc.DepthOrArraySize = 1;								// 1 : �o�b�t�@�̏ꍇ��1���w��A�e�N�X�`���̏ꍇ�͔z�񐔂��w��A�{�����[���e�N�X�`���̏ꍇ�͉��s�����w��
		resDesc.MipLevels = 1;										// 1 : �~�b�v�}�b�v���x�������w��
		resDesc.Format = DXGI_FORMAT_UNKNOWN;						// DXGI_FORMAT_UNKNOWN : �t�H�[�}�b�g���w��
		resDesc.SampleDesc.Count = 1;								// 1 : �T���v�����O�����w��
		resDesc.SampleDesc.Quality = 0;								// 0 : �T���v�����O�i�����w��
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;			// D3D12_TEXTURE_LAYOUT_ROW_MAJOR : �s�D��ŕۑ����ꂽ�e�N�X�`���f�[�^���w��
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;					// D3D12_RESOURCE_FLAG_NONE : �I�v�V��������Ɏw�肵�Ȃ�

		// ���\�[�X�̐���
		// ��1������HEAP_PROPERTIES�\���̂̃|�C���^
		// ��2������D3D12_HEAP_FLAG�񋓌^�̒l
		// ��3������D3D12_RESOURCE_DESC�\���̂̃|�C���^
		// ��4�����̓��\�[�X�̏�����Ԃ�ݒ�
		// ��5�����̓����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���e�N�X�`���̂��߂̃I�v�V����
		// ��6�����̓C���^�[�t�F�XID
		auto hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : �q�[�v�̃I�v�V��������Ɏw�肵�Ȃ�
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,							// D3D12_RESOURCE_DIMENSION_BUFFER���w�肵���ꍇ��nullptr���w��
			IID_PPV_ARGS(m_pIB.ReleaseAndGetAddressOf()));

		if (FAILED(hr))
		{	// ���s��
			assert(0 && "[Polygon.cpp]�C���f�b�N�X�o�b�t�@�̐����Ɏ��s���܂����B");
			return false;
		}

		// �}�b�s���O
		void* p = nullptr;
		// �������̓T�u���\�[�X�̃C���f�b�N�X
		// �������̓}�b�s���O�̊J�n�I�t�Z�b�g�ƏI���I�t�Z�b�g��ݒ�ł���Bnullptr���w�肷��ƑS�̂��}�b�s���O
		// ��O�����̓��\�[�X�f�[�^�ւ̃|�C���^���󂯎�郁�����u���b�N�ւ̃|�C���^
		hr = m_pIB->Map(0, nullptr, &p);
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]�C���f�b�N�X�o�b�t�@�̃}�b�s���O�Ɏ��s���܂����B");
			return false;
		}

		// �C���f�b�N�X�f�[�^���}�b�s���O��ɐݒ�
		memcpy(p, indices, sizeof(indices));

		// �}�b�v���O����
		m_pIB->Unmap(0, nullptr);

		// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
		m_ibView.BufferLocation = m_pIB->GetGPUVirtualAddress();	// �o�b�t�@��GPU���z�A�h���X���擾
		m_ibView.Format = DXGI_FORMAT_R32_UINT;					// �C���f�b�N�X�̃t�H�[�}�b�g��ݒ�
		m_ibView.SizeInBytes = static_cast<UINT>(sizeof(indices));	// �C���f�b�N�X�o�b�t�@�S�̂̃T�C�Y��ݒ�
	}

	// �萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�̐���
	// �V���O���o�b�t�@��Ԃ���GPU�������s���ɒ萔�o�b�t�@�����������鋰�ꂪ����̂Ń_�u���o�b�t�@��
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV : �萔�o�b�t�@�A�V�F�[�_���\�[�X�r���[�A�A���I�[�_�[�h�A�N�Z�X�r���[���i�[����q�[�v
		desc.NumDescriptors = 4;								// 1�t���[��������1�ŏ\�������_�u���o�b�t�@�p��2�m��
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : �V�F�[�_���猩����q�[�v
		desc.NodeMask = 0;

		auto hr = RENDERER.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapCBV_SRV_UAV.ReleaseAndGetAddressOf()));
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
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;						// D3D12_HEAP_TYPE_UPLOAD : CPU����GPU�ւ̓]��������
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPU�y�[�W�v���p�e�B�𖢎w��
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : �������v�[�����w��
		heapProp.CreationNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��
		heapProp.VisibleNodeMask = 1;								// �P��GPU�ɂ�铮���O��Ƃ��Ă��邽��1���w��

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

		for (int i = 0; i < 4; ++i)
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
			auto handleCPU = m_pHeapCBV_SRV_UAV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV_SRV_UAV->GetGPUDescriptorHandleForHeapStart();

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

			constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

			// �ϊ��s��̐ݒ�
			m_CBV[i].pBuffer->mWorld = DirectX::XMMatrixIdentity();
			m_CBV[i].pBuffer->mView = DirectX::XMMatrixLookAtLH(eye, focus, up);
			m_CBV[i].pBuffer->mProj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, 1.0f, 1000.0f);
		}
	}

	// ���[�g�V�O�l�`���̐���
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// ���[�g�p�����[�^�̐ݒ�
		D3D12_ROOT_PARAMETER param[2] = {};
		param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// D3D12_ROOT_PARAMETER_TYPE_CBV : �萔�o�b�t�@�r���[
		param[0].Descriptor.ShaderRegister = 0;
		param[0].Descriptor.RegisterSpace = 0;
		param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_DESCRIPTOR_RANGE range = {};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// D3D12_DESCRIPTOR_RANGE_TYPE_CBV : �萔�o�b�t�@�r���[
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = 0;

		param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE : �e�[�u��
		param[1].DescriptorTable.NumDescriptorRanges = 1;
		param[1].DescriptorTable.pDescriptorRanges = &range;
		param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// �X�^�e�B�b�N�T���v���[�̐ݒ�
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					// D3D12_FILTER_MIN_MAG_MIP_LINEAR : �e�N�X�`���̊g��k���ɐ��`��Ԃ��g�p
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;				// D3D12_TEXTURE_ADDRESS_MODE_CLAMP : �e�N�X�`�����W��0.0f�`1.0f�͈̔͊O�ɂȂ����ꍇ�A0.0f�܂���1.0f�ɃN�����v
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;				// D3D12_TEXTURE_ADDRESS_MODE_CLAMP : �e�N�X�`�����W��0.0f�`1.0f�͈̔͊O�ɂȂ����ꍇ�A0.0f�܂���1.0f�ɃN�����v
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;				// D3D12_TEXTURE_ADDRESS_MODE_CLAMP : �e�N�X�`�����W��0.0f�`1.0f�͈̔͊O�ɂȂ����ꍇ�A0.0f�܂���1.0f�ɃN�����v
		sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;					// D3D12_DEFAULT_MIP_LOD_BIAS : �~�b�v�}�b�v���x���̃o�C�A�X��ݒ肵�Ȃ�
		sampler.MaxAnisotropy = 1;											// 1 : �ٕ����t�B���^�����O���g�p���Ȃ�
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;				// D3D12_COMPARISON_FUNC_NEVER : ��r���s��Ȃ�
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK : �e�N�X�`���̋��E�F�𓧖����ɐݒ�
		sampler.MinLOD = -D3D12_FLOAT32_MAX;								// �ŏ��̃~�b�v�}�b�v���x����ݒ�
		sampler.MaxLOD = +D3D12_FLOAT32_MAX;								// �ő�̃~�b�v�}�b�v���x����ݒ�
		sampler.ShaderRegister = 0;											// ���W�X�^�X���b�g0�ɐݒ�
		sampler.RegisterSpace = 0;											// ���W�X�^�X�y�[�X0�ɐݒ�
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			// �s�N�Z���V�F�[�_����̂݃A�N�Z�X�\

		// ���[�g�V�O�l�`���̐ݒ�
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = 2;
		desc.NumStaticSamplers = 1;
		desc.pParameters = param;
		desc.pStaticSamplers = &sampler;
		desc.Flags = flag;

		ComPtr<ID3DBlob> pBlob;
		ComPtr<ID3DBlob> pErrorBlob;

		// �V���A���C�Y
		auto hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			pBlob.ReleaseAndGetAddressOf(),
			pErrorBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]���[�g�V�O�l�`���̃V���A���C�Y�Ɏ��s���܂����B");
			return false;
		}

		// ���[�g�V�O�l�`���̐���
		hr = RENDERER.GetDevice()->CreateRootSignature(
			0,								// GPU�m�[�h�}�X�N
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			IID_PPV_ARGS(m_pRootSignature.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]���[�g�V�O�l�`���̐����Ɏ��s���܂����B");
			return false;
		}
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		// ���̓��C�A�E�g�̐ݒ�
		D3D12_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
			{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		};

		// ���X�^���C�U�X�e�[�g�̐ݒ�
		D3D12_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D12_FILL_MODE_SOLID;								// ���_�ɂ���Č`����Ă���O�p�`�ŕ`��
		rsDesc.CullMode = D3D12_CULL_MODE_NONE;									// �J�����O���Ȃ�	
		rsDesc.FrontCounterClockwise = FALSE;									// ���v��肪�\��
		rsDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;							// �g�p���Ȃ��̂Ńf�t�H���g
		rsDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;					// �g�p���Ȃ��̂Ńf�t�H���g
		rsDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;	// �g�p���Ȃ��̂Ńf�t�H���g
		rsDesc.DepthClipEnable = FALSE;											// �g�p���Ȃ��̂�FALSE
		rsDesc.MultisampleEnable = FALSE;										// �g�p���Ȃ��̂�FALSE
		rsDesc.AntialiasedLineEnable = FALSE;									// �g�p���Ȃ��̂�FALSE
		rsDesc.ForcedSampleCount = 0;											// 0 : �T���v��������������Ȃ�
		rsDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF : �ێ�I�ȃ��X�^���C�[�[�V�����𖳌��ɂ���

		// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
		D3D12_RENDER_TARGET_BLEND_DESC rtDesc =
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		// �u�����h�X�e�[�g�̐ݒ�
		D3D12_BLEND_DESC bsDesc = {};
		bsDesc.AlphaToCoverageEnable = FALSE;
		bsDesc.IndependentBlendEnable = FALSE;
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			bsDesc.RenderTarget[i] = rtDesc;
		}

		// �[�x�X�e���V���X�e�[�g�̐ݒ�
		D3D12_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;

		ComPtr<ID3DBlob> pVSBlob;
		ComPtr<ID3DBlob> pPSBlob;

		std::wstring vsPath;
		std::wstring psPath;

		if (!SearchFilePath(L"Assets/Shaders/VS_Texture.cso", vsPath))
		{
			assert(0 && "[Polygon.cpp]���_�V�F�[�_�[��������܂���ł���");
			return false;
		}
		if (!SearchFilePath(L"Assets/Shaders/PS_Texture.cso", psPath))
		{
			assert(0 && "[Polygon.cpp]�s�N�Z���V�F�[�_�[��������܂���ł���");
			return false;
		}

		// ���_�V�F�[�_�[�̓ǂݍ���
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]���_�V�F�[�_�[�̓ǂݍ��݂Ɏ��s���܂����B");
			return false;
		}
		// �s�N�Z���V�F�[�_�[�̓ǂݍ���
		hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]�s�N�Z���V�F�[�_�[�̓ǂݍ��݂Ɏ��s���܂����B");
			return false;
		}

		// �p�C�v���C���X�e�[�g�̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { layout,_countof(layout) };
		desc.pRootSignature = m_pRootSignature.Get();
		desc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
		desc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
		desc.RasterizerState = rsDesc;
		desc.BlendState = bsDesc;
		desc.DepthStencilState = dsDesc;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// �p�C�v���C���X�e�[�g�̐���
		hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPSO.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]�p�C�v���C���X�e�[�g�̐����Ɏ��s���܂����B");
			return false;
		}

	}

	// �e�N�X�`���̐���
	if (!CreateTexture())
	{
		assert(0 && "[Polygon.cpp]�e�N�X�`���̐����Ɏ��s���܂����B");
		return false;
	}

	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(WIDTH);
		m_viewport.Height = static_cast<float>(HEIGHT);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.left = 0;
		m_scissorRect.right = WIDTH;
		m_scissorRect.top = 0;
		m_scissorRect.bottom = HEIGHT;
	}
	return true;
}

void Texture::Draw()
{
	// �X�V����
	{
		m_RotateAngle += 0.025f;
		m_CBV[RENDERER.GetFrameIndex() * 2 + 0].pBuffer->mWorld = DirectX::XMMatrixRotationZ(m_RotateAngle + DirectX::XMConvertToRadians(45.0f));
		m_CBV[RENDERER.GetFrameIndex() * 2 + 1].pBuffer->mWorld = DirectX::XMMatrixRotationY(m_RotateAngle) * DirectX::XMMatrixScaling(2.0f, 0.5f, 1.0f);
	}

	// �`�揈��
	{
		auto cmdList = RENDERER.GetCmdList();
		cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
		cmdList->SetDescriptorHeaps(1, m_pHeapCBV_SRV_UAV.GetAddressOf());
		cmdList->SetPipelineState(m_pPSO.Get());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &m_vbView);
		cmdList->IASetIndexBuffer(&m_ibView);
		cmdList->RSSetViewports(1, &m_viewport);
		cmdList->RSSetScissorRects(1, &m_scissorRect);


		uint32_t frameIndex = RENDERER.GetFrameIndex();
		// ��O�̃|���S��
		cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[frameIndex * 2 + 0].Desc.BufferLocation);
		cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.HandleGPU);
		// �`��
		// ��1�����̓C���f�b�N�X�� : 1�̃C���f�b�N�X��1�̒��_���w�����߁A���_���Ɠ���6���w��
		// ��2�����̓C���X�^���X�� : �l�p�`��̂���1���w��
		// ��3�����͊J�n�C���f�b�N�X : �擪�f�[�^����`�悷�邽��0���w��
		// ��4�����͊J�n���_ : 0���w��
		// ��5�����̓C���X�^���X�̊J�n�C���f�b�N�X : �ŏ��̃C���X�^���X����`�悷�邽��0���w��
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		// ���̃|���S��
		cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[frameIndex * 2 + 1].Desc.BufferLocation);
		cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.HandleGPU);
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Texture::Uninit()
{
	// �}�b�s���O����
	for (int i = 0; i < 2; ++i)
	{
		if (m_pCB[i])
		{
			m_pCB[i]->Unmap(0, nullptr);
			memset(&m_CBV[i], 0, sizeof(m_CBV[i]));
		}
		m_pCB[i].Reset();
	}
	m_pVB.Reset();
	m_pPSO.Reset();
}

bool Texture::CreateTexture()
{
	// �t�@�C���p�X������
	std::wstring texPath;
	if (!SearchFilePath(L"Assets/Texture/SampleTexture.dds", texPath))
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
			assert(0 && "[Texture.cpp]�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B");
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
			IID_PPV_ARGS(m_Texture.pResource.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]�e�N�X�`�����\�[�X�̐����Ɏ��s���܂����B");
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
			IID_PPV_ARGS(m_Texture.pResource.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]�e�N�X�`���A�b�v���[�h�p���\�[�X�̐����Ɏ��s���܂����B");
			return false;
		}

		// �e�N�X�`���f�[�^�̃R�s�[
		hr = m_Texture.pResource->WriteToSubresource(0, nullptr, img->pixels, img->rowPitch, img->slicePitch);
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]�e�N�X�`���f�[�^�̃R�s�[�Ɏ��s���܂����B");
			return false;
		}
	}

	// �C���N�������g�T�C�Y���擾
	auto incrementSize = RENDERER.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// CPU�n���h����GPU�n���h�����擾
	auto handleCPU = m_pHeapCBV_SRV_UAV->GetCPUDescriptorHandleForHeapStart();
	auto handleGPU = m_pHeapCBV_SRV_UAV->GetGPUDescriptorHandleForHeapStart();

	// �e�N�X�`���Ƀf�B�X�N���v�^�����蓖�Ă�
	handleCPU.ptr += incrementSize * 2;
	handleGPU.ptr += incrementSize * 2;

	m_Texture.HandleCPU = handleCPU;
	m_Texture.HandleGPU = handleGPU;

	// �e�N�X�`���̍\���ݒ���擾
	auto desc = m_Texture.pResource->GetDesc();

	// �V�F�[�_���\�[�X�r���[�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// �V�F�[�_���\�[�X�r���[�̐���
	RENDERER.GetDevice()->CreateShaderResourceView(m_Texture.pResource.Get(), &srvDesc, m_Texture.HandleCPU);

	return true;
}
