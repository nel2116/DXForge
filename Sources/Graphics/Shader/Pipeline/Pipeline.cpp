// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Pipeline.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 18:00
// �T�v   : �p�C�v���C���N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Pipeline.h"
#include "../RootSignature/RootSignature.h"

// ====== �����o�֐� ======
void Pipeline::SetRenderSetting(Renderer* pDev, RootSignature* pRootSignature, std::vector<InputLayout>& inputLayouts, CullMode cullMode, BlendMode blendMode, PrimitiveTopology topology)
{
	m_pRenderer = pDev;
	m_pRootSignature = pRootSignature;
	m_inputLayouts = inputLayouts;
	m_cullMode = cullMode;
	m_blendMode = blendMode;
	m_topologyType = topology;
}

void Pipeline::Create(std::vector<ID3DBlob*> pBlob, const std::vector<DXGI_FORMAT> formats, bool isDepth, bool isDepthMask, int rtvCount, bool isWireFrame)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
	SetInputLayout(inputLayouts, m_inputLayouts);

	// �p�C�v���C���X�e�[�g�̊e��ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// ���_�V�F�[�_��ݒ�
	psoDesc.VS.pShaderBytecode = pBlob[0]->GetBufferPointer();
	psoDesc.VS.BytecodeLength = pBlob[0]->GetBufferSize();

	// �n���V�F�[�_��ݒ�
	if (pBlob[1])
	{
		psoDesc.HS.pShaderBytecode = pBlob[1]->GetBufferPointer();
		psoDesc.HS.BytecodeLength = pBlob[1]->GetBufferSize();
	}
	// �h���C���V�F�[�_��ݒ�
	if (pBlob[2])
	{
		psoDesc.DS.pShaderBytecode = pBlob[2]->GetBufferPointer();
		psoDesc.DS.BytecodeLength = pBlob[2]->GetBufferSize();
	}
	// �W�I���g���V�F�[�_��ݒ�
	if (pBlob[3])
	{
		psoDesc.GS.pShaderBytecode = pBlob[3]->GetBufferPointer();
		psoDesc.GS.BytecodeLength = pBlob[3]->GetBufferSize();
	}

	// �s�N�Z���V�F�[�_��ݒ�
	psoDesc.PS.pShaderBytecode = pBlob[4]->GetBufferPointer();
	psoDesc.PS.BytecodeLength = pBlob[4]->GetBufferSize();

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.RasterizerState.MultisampleEnable = false;

	// �J�����O���[�h��ݒ�
	psoDesc.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(m_cullMode);

	// �t�B���^�[���[�h��ݒ�
	if (isWireFrame)
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;	// ���C���[�t���[��
	}
	else
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	}

	// �[�x�ݒ���Z�b�g
	if (isDepth)
	{
		psoDesc.RasterizerState.DepthClipEnable = true;
		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.DepthStencilState.DepthWriteMask = isDepthMask ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	}
	else
	{
		psoDesc.RasterizerState.DepthClipEnable = false;
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.StencilEnable = false;
	}

	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;

	// �u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
	SetBlendMode(blendDesc, m_blendMode);

	psoDesc.BlendState.RenderTarget[0] = blendDesc;
	psoDesc.InputLayout.pInputElementDescs = inputLayouts.data();	// ���C�A�E�g�擪�A�h���X
	psoDesc.InputLayout.NumElements = (int)inputLayouts.size();		// ���C�A�E�g�z��̗v�f��

	// �v���~�e�B�u�g�|���W��ݒ�
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = (pBlob[3] && pBlob[4]) ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH : static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_topologyType);

	// RTV�̐���ݒ�
	psoDesc.NumRenderTargets = rtvCount;

	// RTV�̃t�H�[�}�b�g��ݒ�
	for (int i = 0; i < rtvCount; ++i)
	{
		psoDesc.RTVFormats[i] = formats[i];
	}

	psoDesc.SampleDesc.Count = 1;	// �T���v�����O��1�s�N�Z���ɂ�1
	psoDesc.SampleDesc.Quality = 0;	// �N�I���e�B�͍Œ�
	psoDesc.pRootSignature = m_pRootSignature->GetRootSignature();	// ���[�g�V�O�l�`����ݒ�

	// �p�C�v���C���X�e�[�g�𐶐�
	auto hr = m_pRenderer->GetDev()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(0 && "�p�C�v���C���X�e�[�g�̍쐬�Ɏ��s���܂����B");
		return;
	}
}

void Pipeline::SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElements, std::vector<InputLayout>& inputLayouts)
{
	for (int i = 0; i < (int)inputLayouts.size(); ++i)
	{
		if (inputLayouts[i] == InputLayout::POSITION)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
		else if (inputLayouts[i] == InputLayout::NORMAL)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
		else if (inputLayouts[i] == InputLayout::TEXCOORD)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
		else if (inputLayouts[i] == InputLayout::COLOR)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
		else if (inputLayouts[i] == InputLayout::TANGENT)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
		else if (inputLayouts[i] == InputLayout::SKININDEX)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "SKININDEX", 0, DXGI_FORMAT_R16G16B16A16_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
		else if (inputLayouts[i] == InputLayout::SKINWEIGHT)
		{
			inputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}
	}
}

void Pipeline::SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& blendDesc, BlendMode blendMode)
{
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.BlendEnable = true;

	switch (blendMode)
	{
	case BlendMode::Add:
		// ���Z
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_ONE;

		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		break;

	case BlendMode::Alpha:
		// ������
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		break;

	default:
		break;
	}
}
