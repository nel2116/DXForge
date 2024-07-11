#include "stdafx.h"
#include "Factory.h"

ComPtr<ID3D12PipelineState> Factory::CreatePSO(const std::string& name, const RootSignature* pRootSig, const ID3DBlob* pVS, const ID3DBlob* pPS, const D3D12_INPUT_ELEMENT_DESC* pElement, const D3D12_PRIMITIVE_TOPOLOGY_TYPE topology, const D3D12_DEPTH_STENCIL_DESC depthStencilState, const D3D12_BLEND_DESC blendState, const D3D12_RASTERIZER_DESC rasterizerState, const UINT sampleMask, const DXGI_SAMPLE_DESC sampleDesc, const UINT numRenderTargets, const DXGI_FORMAT rtvFormat, const DXGI_FORMAT dsvFormat, const D3D12_DSV_FLAGS depthStencilViewFlags)
{
	ComPtr<ID3D12PipelineState> pPSO;
	// パイプラインステートの生成
	{
		unsigned int elemrntCount = sizeof(pElement) / sizeof(D3D12_INPUT_ELEMENT_DESC);

		// パイプラインステートの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { pElement , elemrntCount };
		desc.pRootSignature = pRootSig->GetPtr();
		desc.VS = { pVS->GetBufferPointer(), pVS->GetBufferSize() };
		desc.PS = { pPS->GetBufferPointer(), pPS->GetBufferSize() };
		desc.RasterizerState = rasterizerState;
		desc.BlendState = blendState;
		desc.DepthStencilState = depthStencilState;
		desc.SampleMask = sampleMask;
		desc.PrimitiveTopologyType = topology;
		desc.NumRenderTargets = numRenderTargets;
		desc.RTVFormats[0] = rtvFormat;
		desc.DSVFormat = dsvFormat;
		desc.SampleDesc = sampleDesc;

		// パイプラインステートの生成
		auto hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pPSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("[Factory.cpp]Error : Line30 : パイプラインステートの生成に失敗しました。");
			return nullptr;
		}
	}
	m_pPSO[name] = pPSO;
	return pPSO;
}


void Factory::SetRootSignature(const std::string& name, RootSignature* pRootSig)
{
	m_pRootSig[name] = pRootSig;
}

RootSignature* Factory::GetRootSignature(const std::string& name)
{
	// 名前を元にルートシグネチャを取得
	auto it = m_pRootSig.find(name);
	// ルートシグネチャが存在する場合
	if (it != m_pRootSig.end())
	{
		// ルートシグネチャを返す
		return it->second;
	}
	// ルートシグネチャが存在しない場合
	// nullptrを返す
	return nullptr;
}

ID3D12PipelineState* Factory::GetPSO(const std::string& name)
{
	// 名前を元にパイプラインステートを取得
	auto it = m_pPSO.find(name);
	// パイプラインステートが存在する場合
	if (it != m_pPSO.end())
	{
		// パイプラインステートを返す
		return it->second.Get();
	}
	// パイプラインステートが存在しない場合
	// nullptrを返す
	return nullptr;
}

