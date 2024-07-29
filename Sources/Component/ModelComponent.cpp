#include "stdafx.h"
#include "ModelComponent.h"

ModelComponent::~ModelComponent()
{
}

void ModelComponent::Init()
{
}

void ModelComponent::Update()
{
	int frameIndex = RENDERER.GetFrameIndex();

	// ���b�V���̃��[���h�s��̍X�V
	{
		auto ptr = m_MeshCB[frameIndex].GetPtr<CbMesh>();
		ptr->World = Matrix::Identity;
	}
}

void ModelComponent::Draw()
{
	// ���f����`��.
	auto pCmd = RENDERER.GetCmdList()->Get();
	int frameIndex = RENDERER.GetFrameIndex();
	pCmd->SetGraphicsRootDescriptorTable(1, m_MeshCB[frameIndex].GetHandleGPU());
	for (size_t i = 0; i < m_pMesh.size(); ++i)
	{
		// �}�e���A��ID���擾.
		auto id = m_pMesh[i]->GetMaterialId();

		// �e�N�X�`����ݒ�.
		pCmd->SetGraphicsRootDescriptorTable(4, m_Material.GetTextureHandle(id, TU_BASE_COLOR));
		pCmd->SetGraphicsRootDescriptorTable(5, m_Material.GetTextureHandle(id, TU_METALLIC));
		pCmd->SetGraphicsRootDescriptorTable(6, m_Material.GetTextureHandle(id, TU_ROUGHNESS));
		pCmd->SetGraphicsRootDescriptorTable(7, m_Material.GetTextureHandle(id, TU_NORMAL));

		// ���b�V����`��.
		m_pMesh[i]->Draw();
	}
}

void ModelComponent::Uninit()
{
	// ���b�V���̉��
	for (auto& mesh : m_pMesh)
	{
		SAFE_UNINIT(mesh);
	}
	m_pMesh.clear();
	m_pMesh.shrink_to_fit();

	// �}�e���A���̉��
	m_Material.Uninit();
}

void ModelComponent::LoadModel(std::string fileName)
{
	// ���b�V�������[�h
	{

		std::wstring path;
		std::wstring filePath = StringToWString(fileName);
		// �t�@�C���p�X������

		if (!SearchFilePathW(filePath.c_str(), path))
		{
			ELOG("[App.cpp]Error : Line81 : �t�@�C����������܂���ł����B");
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// ���b�V���̓ǂݍ���
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line93 : ���b�V���̓ǂݍ��݂Ɏ��s���܂����BFilePath = %ls", path.c_str());
		}

		// ��������\��
		m_pMesh.reserve(resMesh.size());

		// ���b�V����������
		for (size_t i = 0; i < resMesh.size(); ++i)
		{
			// ���b�V���̐���
			auto pMesh = NEW Mesh();

			// �`�F�b�N
			if (!pMesh)
			{
				ELOG("[App.cpp]Error : Line109 : �������̊m�ۂɎ��s���܂����B");
			}

			// ����������
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line116 : ���b�V���̏������Ɏ��s���܂����B");
				SAFE_DELETE(pMesh);
			}

			// ���b�V���̒ǉ�
			m_pMesh.push_back(pMesh);
		}

		// �������œK��
		resMesh.shrink_to_fit();

		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);

		// �}�e���A���̏�����
		if (!m_Material.Init(pPool, sizeof(CbMaterial), resMaterial.size()))
		{
			ELOG("[App.cpp]Error : Line133 : �}�e���A���̏������Ɏ��s���܂����B");
		}

		// �e�N�X�`���ƃ}�e���A����ݒ�.
		{
			/* �����ł̓}�e���A�������ߑł��ł��邱�Ƃ�O��Ƀn�[�h�R�[�f�B���O���Ă��܂�. */
			m_Material.SetTexture(0, TU_BASE_COLOR, dir + L"wall_bc.dds");
			m_Material.SetTexture(0, TU_METALLIC, dir + L"wall_m.dds");
			m_Material.SetTexture(0, TU_ROUGHNESS, dir + L"wall_r.dds");
			m_Material.SetTexture(0, TU_NORMAL, dir + L"wall_n.dds");

			m_Material.SetTexture(1, TU_BASE_COLOR, dir + L"matball_bc.dds");
			m_Material.SetTexture(1, TU_METALLIC, dir + L"matball_m.dds");
			m_Material.SetTexture(1, TU_ROUGHNESS, dir + L"matball_r.dds");
			m_Material.SetTexture(1, TU_NORMAL, dir + L"matball_n.dds");
		}
	}

	// ���b�V���p�o�b�t�@�̐���.
	{
		DescriptorPool* pPool = RENDERER.GetPool(POOL_TYPE_RES);
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_MeshCB[i].Init(pPool, sizeof(CbMesh)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
			}

			auto ptr = m_MeshCB[i].GetPtr<CbMesh>();
			ptr->World = Matrix::Identity;
		}
	}
}
