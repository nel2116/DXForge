#include "stdafx.h"
#include "PMDActor.h"
#include <System/PMDRenderer.h>
#include <System/Dx12Wrapper.h>
#include <DirectX/d3dx12.h>
using namespace Microsoft::WRL;
using namespace DirectX;

namespace
{
	/// <summary>
	/// �e�N�X�`���̃p�X���Z�p���[�^�����ŕ�������
	/// @param[in] path �e�N�X�`���̃p�X
	/// @param[in] sepitter �Z�p���[�^����
	/// @return �����O��̕�����y�A
	/// </summary>
	pair<string, string> SplitFileName(const string& path, const char sepitter = '*')
	{
		int idx = path.find(sepitter);
		pair<string, string> ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr(idx + 1, path.length() - idx - 1);
		return ret;
	}

	/// <summary>
	/// �t�@�C��������g���q���擾����
	/// @param filePath : �t�@�C���p�X
	/// @return : �t�@�C���̊g���q
	/// </summary>
	string GetExtension(const string& filePath)
	{
		int idx = filePath.rfind('.');
		return filePath.substr(idx + 1, filePath.length() - idx - 1);
	}

	/// <summary>
	/// ���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
	/// @param modelPath : �A�v���P�[�V�������猩��PMD���f���̃p�X
	/// @param texPath : PMD���f�����猩���e�N�X�`���̃p�X
	/// @return : �A�v���P�[�V�������猩���e�N�X�`���̃p�X
	/// </summary>
	string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath)
	{
		// �t�@�C���̃t�H���_��؂��\��/�̂Ɏ�ނ��g�p�����\��������
		// �Ƃ�����������\��/�𓾂���΂����̂ŁA�o����rfind�����A��r����
		// int�^�������Ă���̂͌�����Ȃ������ꍇ��rfind��epos(-1��0xffffffff)��Ԃ�����
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}
}

void* PMDActor::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

PMDActor::PMDActor(const char* path, PMDRenderer& renderer)
	: m_renderer(renderer)
	, m_dx12(renderer.m_dx12)
	, _angle(0.0f)
{
	m_transform.world = XMMatrixIdentity();
	assert(SUCCEEDED(LoadPMDFile(path)));
	assert(SUCCEEDED(CreateTransformView()));
	assert(SUCCEEDED(CreateMaterialData()));
	assert(SUCCEEDED(CreateMaterialAndTextureView()));
}

PMDActor::~PMDActor()
{
}

PMDActor* PMDActor::Clone()
{
	return nullptr;
}

void PMDActor::Update()
{
	_angle += 0.03f;
	m_transform.world = XMMatrixRotationY(_angle);
}

void PMDActor::Draw()
{
	m_dx12.GetCommandList()->IASetVertexBuffers(0, 1, &m_vbView);
	m_dx12.GetCommandList()->IASetIndexBuffer(&m_ibView);

	ID3D12DescriptorHeap* transheaps[] = { m_transfromHeap.Get() };
	// �g�����X�t�H�[���q�[�v���Z�b�g
	m_dx12.GetCommandList()->SetDescriptorHeaps(1, transheaps);
	m_dx12.GetCommandList()->SetGraphicsRootDescriptorTable(1, m_transfromHeap->GetGPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* mdh[] = { m_materialHeap.Get() };
	// �}�e���A���q�[�v���Z�b�g
	m_dx12.GetCommandList()->SetDescriptorHeaps(1, mdh);

	auto materialH = m_materialHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;

	auto cbvsrvIncSize = m_dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
	for (auto& m : m_materials)
	{
		m_dx12.GetCommandList()->SetGraphicsRootDescriptorTable(2, materialH);
		m_dx12.GetCommandList()->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);
		materialH.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}

HRESULT PMDActor::CreateMaterialData()
{
	// �}�e���A���o�b�t�@���쐬
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * m_materials.size());
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_materialBuff.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	// �}�b�v�}�e���A���ɃR�s�[
	char* mapMaterial = nullptr;
	hr = m_materialBuff->Map(0, nullptr, (void**)&mapMaterial);
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	for (auto& m : m_materials)
	{
		*((MaterialForHlsl*)mapMaterial) = m.material;	// �R�s�[
		mapMaterial += materialBuffSize;				// �|�C���^��i�߂�
	}
	m_materialBuff->Unmap(0, nullptr);

	return S_OK;
}

HRESULT PMDActor::CreateMaterialAndTextureView()
{
	D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
	materialDescHeapDesc.NumDescriptors = m_materials.size() * 5;	// �}�e���A������(�萔1�A�e�N�X�`��3��)
	materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	materialDescHeapDesc.NodeMask = 0;
	materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// �f�X�N���v�^�q�[�v�̎��
	auto hr = m_dx12.GetDevice()->CreateDescriptorHeap(&materialDescHeapDesc, IID_PPV_ARGS(m_materialHeap.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = m_materialBuff->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = materialBuffSize;

	// �}�e���A���̃r���[���쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;						// �~�b�v�}�b�v��
	CD3DX12_CPU_DESCRIPTOR_HANDLE matDescHeapH(m_materialHeap->GetCPUDescriptorHandleForHeapStart());
	auto incSize = m_dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < m_materials.size(); ++i)
	{
		// �}�e���A���Œ�o�b�t�@�r���[
		m_dx12.GetDevice()->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += incSize;
		matCBVDesc.BufferLocation += materialBuffSize;
		if (m_textureResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_whiteTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_textureResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_textureResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.Offset(incSize);

		if (m_sphResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_whiteTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_sphResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_sphResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (m_spaResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_blackTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_spaResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_spaResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (m_toonResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_gradTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_toonResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_toonResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;
	}

	return S_OK;
}

HRESULT PMDActor::CreateTransformView()
{
	// GPU�o�b�t�@�쐬
	auto buffSize = sizeof(Transform);
	buffSize = (buffSize + 0xff) & ~0xff;
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_transformBuff.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	// �}�b�v�ƃR�s�[
	hr = m_transformBuff->Map(0, nullptr, (void**)&m_mappedTransform);
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	*m_mappedTransform = m_transform;

	// �r���[�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformHeapDesc = {};
	transformHeapDesc.NumDescriptors = 1;	// 
	transformHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformHeapDesc.NodeMask = 0;
	transformHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// �f�X�N���v�^�q�[�v�̎��
	hr = m_dx12.GetDevice()->CreateDescriptorHeap(&transformHeapDesc, IID_PPV_ARGS(m_transfromHeap.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_transformBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	m_dx12.GetDevice()->CreateConstantBufferView(&cbvDesc, m_transfromHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

HRESULT PMDActor::LoadPMDFile(const char* path)
{
	// PMD�w�b�_�\����
	struct PMDHeader
	{
		float version;			// �o�[�W���� (��:00 00 80 3F == 1.00)
		char model_name[20];	// ���f����
		char comment[256];		// ���f���R�����g
	};
	char signature[3];
	PMDHeader pmdheader = {};

	string strModelPath = path;

	auto fp = fopen(strModelPath.c_str(), "rb");
	if (fp == nullptr)
	{	// �G���[����
		assert(0);
		return ERROR_FILE_NOT_FOUND;
	}

	// �V�O�l�`���̓ǂݍ���
	fread(signature, sizeof(signature), 1, fp);
	// �w�b�_�[�̓ǂݍ���
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	unsigned int vertNum = 0;	// ���_��
	fread(&vertNum, sizeof(vertNum), 1, fp);

#pragma pack(1)	// ��������1�o�C�g�p�b�L���O�E�E�E�A���C�����g�������Ȃ�
	// PMD�}�e���A���\����
	struct PMDMaterial
	{
		DirectX::XMFLOAT3 diffuse;	// �f�B�t���[�Y�F
		float alpha;				// ���l
		float specularity;			// �X�y�L�����̋���(��Z�l)
		DirectX::XMFLOAT3 specular;	// �X�y�L�����F
		DirectX::XMFLOAT3 ambient;	// �A���r�G���g�F
		unsigned char toonIdx;		// �g�D�[���ԍ�
		unsigned char edgeFlg;		// �}�e���A�����Ƃ̗֊s�t���O
		unsigned int indicesNum;	// �ʒ��_��
		char texFilePath[20];		// �e�N�X�`���t�@�C����
	};
#pragma pack()	// �E�E�E1�o�C�g�p�b�L���O����

	constexpr unsigned int pmdvertSize = 38;	// PMD���_�\���̂̃T�C�Y
	vector<unsigned char> vertices(vertNum * pmdvertSize);	// ���_�f�[�^
	fread(vertices.data(), vertices.size(), 1, fp);			// ��C�ɓǂݍ���

	unsigned int indicesNum = 0;	// �C���f�b�N�X��
	fread(&indicesNum, sizeof(indicesNum), 1, fp);

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(vertices[0]));

	// UPLOAD(�m�ۂ͉\)
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_vb.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{	// ���b�Z�[�W�{�b�N�X
		MessageBox(nullptr, "���_�o�b�t�@�̐����Ɏ��s���܂���", "PMD�t�@�C���ǂݍ��݃G���[", MB_OK);
		return hr;
	}

	unsigned char* vertMap = nullptr;
	hr = m_vb->Map(0, nullptr, (void**)&vertMap);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{	// ���b�Z�[�W�{�b�N�X
		MessageBox(nullptr, "���_�o�b�t�@�̃}�b�v�Ɏ��s���܂���", "PMD�t�@�C���ǂݍ��݃G���[", MB_OK);
		return hr;
	}
	copy(vertices.begin(), vertices.end(), vertMap);
	m_vb->Unmap(0, nullptr);

	m_vbView.BufferLocation = m_vb->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	m_vbView.SizeInBytes = vertices.size();					// �S�o�C�g��
	m_vbView.StrideInBytes = pmdvertSize;					// 1���_������̃o�C�g��

	vector<unsigned short> indices(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);	// ��C�ɓǂݍ���

	D3D12_RESOURCE_DESC resDescBuff = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0]));

	// �ݒ�́A�o�b�t�@�̃T�C�Y�ȊO�͒��_�o�b�t�@�̐ݒ���g����
	hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDescBuff,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_ib.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{	// ���b�Z�[�W�{�b�N�X
		MessageBox(nullptr, "�C���f�b�N�X�o�b�t�@�̐����Ɏ��s���܂���", "PMD�t�@�C���ǂݍ��݃G���[", MB_OK);
		return hr;
	}

	// ������o�b�t�@�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* idxMap = nullptr;
	hr = m_ib->Map(0, nullptr, (void**)&idxMap);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{	// ���b�Z�[�W�{�b�N�X
		MessageBox(nullptr, "�C���f�b�N�X�o�b�t�@�̃}�b�v�Ɏ��s���܂���", "PMD�t�@�C���ǂݍ��݃G���[", MB_OK);
		return hr;
	}
	copy(indices.begin(), indices.end(), idxMap);
	m_ib->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
	m_ibView.BufferLocation = m_ib->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	m_ibView.Format = DXGI_FORMAT_R16_UINT;					// �C���f�b�N�X�̃t�H�[�}�b�g
	m_ibView.SizeInBytes = indices.size() * sizeof(indices[0]);	// �S�o�C�g��

	unsigned int materialNum = 0;	// �}�e���A����
	fread(&materialNum, sizeof(materialNum), 1, fp);
	m_materials.resize(materialNum);
	m_textureResources.resize(materialNum);
	m_sphResources.resize(materialNum);
	m_spaResources.resize(materialNum);
	m_toonResources.resize(materialNum);

	vector<PMDMaterial> pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);
	// �R�s�[
	for (int i = 0; i < pmdMaterials.size(); ++i)
	{
		m_materials[i].indicesNum = pmdMaterials[i].indicesNum;
		m_materials[i].material.diffuse = pmdMaterials[i].diffuse;
		m_materials[i].material.alpha = pmdMaterials[i].alpha;
		m_materials[i].material.specular = pmdMaterials[i].specular;
		m_materials[i].material.specularity = pmdMaterials[i].specularity;
		m_materials[i].material.ambient = pmdMaterials[i].ambient;
		m_materials[i].additional.toonIdx = pmdMaterials[i].toonIdx;
	}

	for (int i = 0; i < pmdMaterials.size(); ++i)
	{
		// �g�D�[�����\�[�X�̓ǂݍ���
		char toonFilePath[32];
		sprintf(toonFilePath, "toon/toon%02d.bmp", pmdMaterials[i].toonIdx + 1);
		m_toonResources[i] = m_dx12.GetTextureByPath(toonFilePath);

		if (strlen(pmdMaterials[i].texFilePath) == 0)
		{
			ID3D12Resource* tmp = nullptr;
			m_textureResources[i].Swap(tmp);
			SAFE_RELEASE(tmp);
			continue;
		}

		string texFileName = pmdMaterials[i].texFilePath;
		string sphFileName = "";
		string spaFileName = "";
		if (count(texFileName.begin(), texFileName.end(), '*') > 0)
		{	// �X�v���b�^������
			auto namepair = SplitFileName(texFileName);
			if (GetExtension(namepair.first) == "sph")
			{
				texFileName = namepair.second;
				sphFileName = namepair.first;
			}
			else if (GetExtension(namepair.first) == "spa")
			{
				texFileName = namepair.second;
				spaFileName = namepair.first;
			}
			else
			{
				texFileName = namepair.first;
				if (GetExtension(namepair.second) == "sph")
				{
					sphFileName = namepair.second;
				}
				else if (GetExtension(namepair.second) == "spa")
				{
					spaFileName = namepair.second;
				}
			}
		}
		else
		{
			if (GetExtension(pmdMaterials[i].texFilePath) == "sph")
			{
				sphFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else if (GetExtension(pmdMaterials[i].texFilePath) == "spa")
			{
				spaFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else
			{
				texFileName = pmdMaterials[i].texFilePath;
			}
		}
		// ���f���ƃe�N�X�`���p�X����A�v���P�[�V��������̃e�N�X�`���p�X�𓾂�
		if (texFileName != "")
		{
			auto texFilePath = GetTexturePathFromModelAndTexPath(strModelPath, texFileName.c_str());
			m_textureResources[i] = m_dx12.GetTextureByPath(texFilePath.c_str());
		}
		if (sphFileName != "")
		{
			auto sphFilePath = GetTexturePathFromModelAndTexPath(strModelPath, sphFileName.c_str());
			m_sphResources[i] = m_dx12.GetTextureByPath(sphFilePath.c_str());
		}
		if (spaFileName != "")
		{
			auto spaFilePath = GetTexturePathFromModelAndTexPath(strModelPath, spaFileName.c_str());
			m_spaResources[i] = m_dx12.GetTextureByPath(spaFilePath.c_str());
		}
	}
	fclose(fp);
}
