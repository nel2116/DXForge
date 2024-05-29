// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Shader.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 20:00
// �T�v   : �V�F�[�_�[�N���X�̎���
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Shader.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "../Mesh/Mesh.h"

// ====== �֐� ======
namespace
{
	std::wstring StringToWString(const std::string& str)
	{
		//�Ăяo��1���(�����񐔂𓾂�)
		auto num1 = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), -1, nullptr, 0);

		std::wstring wstr;//string��wchar_t��
		wstr.resize(num1);//����ꂽ�����񐔂Ń��T�C�Y

		//�Ăяo��2���(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
		auto num2 = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), -1, &wstr[0], num1);

		assert(num1 == num2);//�ꉞ�`�F�b�N
		return wstr;

	}
}

// ====== �����o�֐� ======
void Shader::Create(Renderer* pDev, std::string& filePath, RenderingSetting& setting, std::vector<RangeType> rangeTypes)
{
	m_pRenderer = pDev;

	LoadShader(filePath);

	m_upRootSignature = std::make_unique<RootSignature>();
	m_upRootSignature->Create(pDev, rangeTypes, m_cbvCount);

	m_upPipeline = std::make_unique<Pipeline>();
	m_upPipeline->SetRenderSetting(pDev, m_upRootSignature.get(), setting.inputLayouts, setting.cullMode, setting.blendMode, setting.primitiveTopology);
	m_upPipeline->Create({ m_pVSBlob,m_pHSBlob,m_pDSBlob,m_pGSBlob,m_pPSBlob }, setting.Formats, setting.isDepth, setting.isDepthMask, setting.RTVCount, setting.isWireFrame);
}

void Shader::Begin(int w, int h)
{
	m_pRenderer->GetCmdList()->SetPipelineState(m_upPipeline->GetPipeline());

	// ���[�g�V�O�l�`���̃Z�b�g
	m_pRenderer->GetCmdList()->SetGraphicsRootSignature(m_upRootSignature->GetRootSignature());

	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_upPipeline->GetTopology());

	switch (topologyType)
	{
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
		m_pRenderer->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;

	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
		m_pRenderer->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;

	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
		m_pRenderer->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;

	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
		m_pRenderer->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		break;
	}

	// �r���[�|�[�g�̐ݒ�
	D3D12_VIEWPORT vp = {};
	D3D12_RECT rect = {};
	vp.Width = static_cast<float>(w);
	vp.Height = static_cast<float>(h);
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	rect.top = 0;
	rect.left = 0;
	rect.right = rect.left + w;
	rect.bottom = rect.top + h;

	m_pRenderer->GetCmdList()->RSSetViewports(1, &vp);
	m_pRenderer->GetCmdList()->RSSetScissorRects(1, &rect);
}

void Shader::DrawMesh(const Mesh& mesh)
{
	SetMaterial(mesh.GetMaterial());
	mesh.DrawInstanced(mesh.GetInstanceCount());
}

void Shader::DrawModel(const Model& model)
{
	for (auto& node : model.GetNodes())
	{
		DrawMesh(*node.spMesh);
	}
}

void Shader::LoadShader(const std::string& filePath)
{
	ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	UINT flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ID3DBlob* error = nullptr;

	std::string format = ".hlsl";
	std::string currentPath = "Sources/HLSL/";

	// ���_�V�F�[�_�[�̃R���p�C��
	{
		std::string vsPath = currentPath + "VertexShader/VS_" + filePath + format;
		HRESULT hr = D3DCompileFromFile(
			StringToWString(vsPath).c_str(),
			nullptr,
			include,
			"VS_Main",
			"vs_5_0",
			flag,
			0,
			&m_pVSBlob,
			&error);

		if (FAILED(hr))
		{
			assert(0 && "���_�V�F�[�_�[�̃R���p�C���Ɏ��s���܂���");
			OutputDebugStringA((char*)error->GetBufferPointer());
			error->Release();
			return;
		}
	}

	// �n���V�F�[�_�[�̃R���p�C��
	{
		std::string hsPath = currentPath + "HullShader/HS_" + filePath + format;
		HRESULT hr = D3DCompileFromFile(
			StringToWString(hsPath).c_str(),
			nullptr,
			include,
			"HS_Main",
			"hs_5_0",
			flag,
			0,
			&m_pHSBlob,
			&error);
		// �n���V�F�[�_�[�͂Ȃ��Ă��ǂ�
	}

	// �h���C���V�F�[�_�[�̃R���p�C��
	{
		std::string dsPath = currentPath + "DomainShader/DS_" + filePath + format;
		HRESULT hr = D3DCompileFromFile(
			StringToWString(dsPath).c_str(),
			nullptr,
			include,
			"DS_Main",
			"ds_5_0",
			flag,
			0,
			&m_pDSBlob,
			&error);
		// �h���C���V�F�[�_�[�͂Ȃ��Ă��ǂ�
	}

	// �W�I���g���V�F�[�_�[�̃R���p�C��
	{
		std::string gsPath = currentPath + "GeometryShader/GS_" + filePath + format;
		HRESULT hr = D3DCompileFromFile(
			StringToWString(gsPath).c_str(),
			nullptr,
			include,
			"GS_Main",
			"gs_5_0",
			flag,
			0,
			&m_pGSBlob,
			&error);
		// �W�I���g���V�F�[�_�[�͂Ȃ��Ă��ǂ�
	}

	// �s�N�Z���V�F�[�_�[�̃R���p�C��
	{
		std::string psPath = currentPath + "PixelShader/PS_" + filePath + format;
		HRESULT hr = D3DCompileFromFile(
			StringToWString(psPath).c_str(),
			nullptr,
			include,
			"PS_Main",
			"ps_5_0",
			flag,
			0,
			&m_pPSBlob,
			&error);

		if (FAILED(hr))
		{
			assert(0 && "�s�N�Z���V�F�[�_�[�̃R���p�C���Ɏ��s���܂���");
			OutputDebugStringA((char*)error->GetBufferPointer());
			error->Release();
			return;
		}
	}
}

void Shader::SetMaterial(const Material& material)
{
	material.spBaseColorTex->Set(m_cbvCount);
	material.spNormalTex->Set(m_cbvCount + 1);
	material.spMetallicRoughnessTex->Set(m_cbvCount + 2);
	material.spEmissiveTex->Set(m_cbvCount + 3);
}
