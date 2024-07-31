// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [PipelineState.cpp]
// 制作者 : 田中ミノル
// 作成日 : 2024/07/29
// 概要   : パイプラインステートクラスの実装
// 更新日
// 2024/07/29 : 田中ミノル : 新規作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "PipelineState.h"

PipelineState::Desc::Desc()
{
	ZeroMemory(&m_psoDesc, sizeof(m_psoDesc));
	ZeroMemory(&m_rasterizerDesc, sizeof(m_rasterizerDesc));
	ZeroMemory(&m_blendDesc, sizeof(m_blendDesc));
	ZeroMemory(&m_depthStencilDesc, sizeof(m_depthStencilDesc));

	// デフォルトのラスタライザーステート設定
	m_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	m_rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	m_rasterizerDesc.FrontCounterClockwise = FALSE;
	m_rasterizerDesc.DepthClipEnable = TRUE;

	// デフォルトのブレンドステート設定
	m_blendDesc.AlphaToCoverageEnable = FALSE;
	m_blendDesc.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultBlendDesc =
	{
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL
	};
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		m_blendDesc.RenderTarget[i] = defaultBlendDesc;
	}

	// デフォルトの深度ステンシルステート設定
	m_depthStencilDesc.DepthEnable = TRUE;
	m_depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	m_depthStencilDesc.StencilEnable = FALSE;

	m_psoDesc.RasterizerState = m_rasterizerDesc;
	m_psoDesc.BlendState = m_blendDesc;
	m_psoDesc.DepthStencilState = m_depthStencilDesc;
	m_psoDesc.SampleMask = UINT_MAX;
	m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_psoDesc.NumRenderTargets = 1;
	m_psoDesc.SampleDesc.Count = 1;
}

PipelineState::Desc::~Desc() = default;

PipelineState::Desc& PipelineState::Desc::SetVertexShader(const D3D12_SHADER_BYTECODE& vs)
{
	m_psoDesc.VS = vs;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetPixelShader(const D3D12_SHADER_BYTECODE& ps)
{
	m_psoDesc.PS = ps;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout)
{
	m_inputLayoutDesc = inputLayout;
	m_psoDesc.InputLayout = m_inputLayoutDesc;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	m_psoDesc.pRootSignature = rootSignature;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
{
	m_psoDesc.RTVFormats[0] = rtvFormat;
	m_psoDesc.DSVFormat = dsvFormat;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerState)
{
	m_psoDesc.RasterizerState = rasterizerState;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetBlendState(const D3D12_BLEND_DESC& blendState)
{
	m_psoDesc.BlendState = blendState;
	return *this;
}

PipelineState::Desc& PipelineState::Desc::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState)
{
	m_psoDesc.DepthStencilState = depthStencilState;
	return *this;
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC* PipelineState::Desc::GetDesc() const
{
	return &m_psoDesc;
}

PipelineState::PipelineState() = default;

PipelineState::~PipelineState()
{
	Uninit();
}

bool PipelineState::Init(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc)
{
	auto pDevice = RENDERER.GetDevice();
	auto hr = pDevice->CreateGraphicsPipelineState(pDesc, IID_PPV_ARGS(m_pPipelineState.GetAddressOf()));
	if (FAILED(hr))
	{
		LogHResult(hr);
		// ELOG("Error : CreateGraphicsPipelineState() Failed. recode = 0x%x", hr);
		return false;
	}
	return true;
}

void PipelineState::Uninit()
{
	m_pPipelineState.Reset();
}

ID3D12PipelineState* PipelineState::GetPtr() const
{
	return m_pPipelineState.Get();
}
