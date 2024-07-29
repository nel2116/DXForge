// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [PipelineState.h]
// 制作者 : 田中ミノル
// 作成日 : 2024/07/29
// 概要   : パイプラインステートクラスの定義
// 更新日
// 2024/07/29 : 田中ミノル : 新規作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "System/ComPtr.h"

// ====== クラスの定義 ======
class PipelineState
{
public:
	class Desc
	{
	public:
		Desc();
		~Desc();
		Desc& SetVertexShader(const D3D12_SHADER_BYTECODE& vs);
		Desc& SetPixelShader(const D3D12_SHADER_BYTECODE& ps);
		Desc& SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout);
		Desc& SetRootSignature(ID3D12RootSignature* rootSignature);
		Desc& SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN);
		Desc& SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerState);
		Desc& SetBlendState(const D3D12_BLEND_DESC& blendState);
		Desc& SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState);
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetDesc() const;

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
		D3D12_RASTERIZER_DESC m_rasterizerDesc;
		D3D12_BLEND_DESC m_blendDesc;
		D3D12_DEPTH_STENCIL_DESC m_depthStencilDesc;
		D3D12_INPUT_LAYOUT_DESC m_inputLayoutDesc;
	};

	PipelineState();
	~PipelineState();
	bool Init(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc);
	void Uninit();
	ID3D12PipelineState* GetPtr() const;

private:
	ComPtr<ID3D12PipelineState> m_pPipelineState;
};