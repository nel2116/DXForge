// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RootSignature.cpp]
// 作成者 : 田中ミノル
// 作成日 : 20124/06/24
// 概要   : ルートシグネチャクラスの実装
// 更新履歴
// 2024/06/24  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "RootSignature.h"

// ====== RootSignature::Desc クラス ======
// コンストラクタ
RootSignature::Desc::Desc()
	: m_Desc()
	, m_Flags(0)
{
	for (auto i = 0; i < 5; ++i)
	{
		m_DenyStage[i] = true;
	}
}

// デストラクタ
RootSignature::Desc::~Desc()
{
	m_Ranges.clear();
	m_Samplers.clear();
	m_Params.clear();
}

// セットアップ開始
RootSignature::Desc& RootSignature::Desc::Begin(int count)
{
	m_Flags = 0;
	for (auto i = 0; i < 5; ++i)
	{
		m_DenyStage[i] = true;
	}

	memset(&m_Desc, 0, sizeof(m_Desc));

	m_Samplers.clear();
	m_Ranges.resize(count);
	m_Params.resize(count);
	return *this;
}

// ステージチェック
void RootSignature::Desc::CheckStage(ShaderStage stage)
{
	auto index = int(stage - 1);
	if (0 <= index && index < 5)
	{
		m_DenyStage[index] = false;
	}
}

// パラメータ設定
void RootSignature::Desc::SetParam(ShaderStage stage, int index, uint32_t reg, D3D12_DESCRIPTOR_RANGE_TYPE type)
{
	if (index >= m_Params.size())
	{
		return;
	}

	m_Ranges[index].RangeType = type;
	m_Ranges[index].NumDescriptors = 1;
	m_Ranges[index].BaseShaderRegister = reg;
	m_Ranges[index].RegisterSpace = 0;
	m_Ranges[index].OffsetInDescriptorsFromTableStart = 0;

	m_Params[index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	m_Params[index].DescriptorTable.NumDescriptorRanges = 1;
	m_Params[index].DescriptorTable.pDescriptorRanges = &m_Ranges[index];
	m_Params[index].ShaderVisibility = D3D12_SHADER_VISIBILITY(stage);
	CheckStage(stage);
}

// 定数バッファビューを設定
RootSignature::Desc& RootSignature::Desc::SetCBV(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
	return *this;
}

// シェーダーリソースビューを設定
RootSignature::Desc& RootSignature::Desc::SetSRV(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	return *this;
}

// アンオーダードアクセスビューを設定
RootSignature::Desc& RootSignature::Desc::SetUAV(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
	return *this;
}

// サンプラーを設定
RootSignature::Desc& RootSignature::Desc::SetSmp(ShaderStage stage, int index, uint32_t reg)
{
	SetParam(stage, index, reg, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
	return *this;
}

// 静的サンプラーを追加
RootSignature::Desc& RootSignature::Desc::AddStaticSmp(ShaderStage stage, uint32_t reg, SamplerState state)
{
	D3D12_STATIC_SAMPLER_DESC desc = {};

	desc.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = D3D12_FLOAT32_MAX;
	desc.ShaderRegister = reg;
	desc.RegisterSpace = 0;
	desc.ShaderVisibility = D3D12_SHADER_VISIBILITY(stage);
	CheckStage(stage);

	switch (state)
	{
	case SamplerState::PointWrap:
	{
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
	break;

	case SamplerState::PointClamp:
	{
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
	break;

	case SamplerState::LinearWrap:
	{
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
	break;

	case SamplerState::LinearClamp:
	{
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
	break;

	case SamplerState::AnisotropicWrap:
	{
		desc.Filter = D3D12_FILTER_ANISOTROPIC;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;
	}
	break;

	case SamplerState::AnisotropicClamp:
	{
		desc.Filter = D3D12_FILTER_ANISOTROPIC;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;
	}
	break;
	}

	m_Samplers.push_back(desc);
	return *this;
}

// 入力レイアウトを許可
RootSignature::Desc& RootSignature::Desc::AllowIL()
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return *this;
}

// ストリーム出力を許可
RootSignature::Desc& RootSignature::Desc::AllowSO()
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
	return *this;
}

// セットアップ終了
RootSignature::Desc& RootSignature::Desc::End()
{
	if (m_DenyStage[0])
	{
		m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	}

	if (m_DenyStage[1])
	{
		m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	}

	if (m_DenyStage[2])
	{
		m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	}

	if (m_DenyStage[3])
	{
		m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	}

	if (m_DenyStage[4])
	{
		m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	}

	m_Desc.NumParameters = UINT(m_Params.size());
	m_Desc.pParameters = m_Params.data();
	m_Desc.NumStaticSamplers = UINT(m_Samplers.size());
	m_Desc.pStaticSamplers = m_Samplers.data();
	m_Desc.Flags = D3D12_ROOT_SIGNATURE_FLAGS(m_Flags);

	return *this;
}

// 記述子を取得
const D3D12_ROOT_SIGNATURE_DESC* RootSignature::Desc::GetDesc() const
{
	return &m_Desc;
}

// ====== RootSignature クラス ======
// コンストラクタ
RootSignature::RootSignature()
{
}

// デストラクタ
RootSignature::~RootSignature()
{
	Uninit();
}

// 初期化処理
bool RootSignature::Init(const D3D12_ROOT_SIGNATURE_DESC* pDesc)
{
	auto pDevice = RENDERER.GetDevice();
	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// シリアライズ
	auto hr = D3D12SerializeRootSignature(
		pDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		pBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("Error : D3D12SerializeRootSignature() Failed. recode = 0x%x", hr);
		return false;
	}

	// ルートシグニチャを生成.
	hr = pDevice->CreateRootSignature(
		0,
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf()));
	if (FAILED(hr))
	{
		ELOG("Error : Root Signature Create Failed. retcode = 0x%x", hr);
		return false;
	}

	return true;
}

// 終了処理
void RootSignature::Uninit()
{
	m_pRootSignature.Reset();
}

// ルートシグネチャの取得
ID3D12RootSignature* RootSignature::GetPtr() const
{
	return m_pRootSignature.Get();
}