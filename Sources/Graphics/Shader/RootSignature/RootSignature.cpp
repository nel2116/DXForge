// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RootSignature.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 18:00
// 概要   : ルートシグネチャークラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "RootSignature.h"

// ====== メンバ関数 ======
// 作成
void RootSignature::Create(Renderer* pDev, std::vector<RangeType>& rangeType)
{
	m_pRenderer = pDev;

	// ルートシグネチャーの作成
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	int rangeCount = (int)rangeType.size();

	// レンジの数分だけルートパラメータ、レンジを作成
	std::vector<D3D12_ROOT_PARAMETER> rootParams(rangeCount);
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges(rangeCount);


	// SRVの数だけSamplerDescを作成
	int samplerCount = 0;
	for (int i = 0; i < (int)rangeType.size(); i++)
	{
		if (rangeType[i] == RangeType::SRV)
		{
			samplerCount++;
		}
	}

	// 指定された順に設定
	samplerCount = 0;
	bool bSampler = false;
	int cbvCount = 0;
	int uavCount = 0;

	for (int i = 0; i < rangeCount; ++i)
	{
		switch (rangeType[i])
		{
		case RangeType::CBV:
			CreateRange(ranges[i], RangeType::CBV, cbvCount);
			rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			cbvCount++;
			break;

		case RangeType::SRV:
			CreateRange(ranges[i], RangeType::SRV, samplerCount);
			rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			samplerCount++;
			bSampler = true;
			break;

		case RangeType::UAV:
			CreateRange(ranges[i], RangeType::UAV, uavCount);
			rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			uavCount++;
			break;

		default:
			break;
		}
	}

	std::array<D3D12_STATIC_SAMPLER_DESC, 4> samplerDescs = {};
	if (bSampler)
	{
		CreateStaticSampler(samplerDescs[0], TextureAddressMode::Wrap, D3D12Filter::Point, 0);
		CreateStaticSampler(samplerDescs[1], TextureAddressMode::Clamp, D3D12Filter::Point, 1);
		CreateStaticSampler(samplerDescs[2], TextureAddressMode::Wrap, D3D12Filter::Linear, 2);
		CreateStaticSampler(samplerDescs[3], TextureAddressMode::Clamp, D3D12Filter::Linear, 3);
	}

	rootSignatureDesc.pStaticSamplers = bSampler ? samplerDescs.data() : nullptr;
	rootSignatureDesc.NumStaticSamplers = bSampler ? (UINT)samplerDescs.size() : 0;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams.data();
	rootSignatureDesc.NumParameters = (UINT)rootParams.size();

	ID3DBlob* pErrorBlob = nullptr;
	auto hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &m_pRootBlob, &pErrorBlob);

	if (FAILED(hr))
	{
		assert(0 && "ルートシグネチャの初期化に失敗。");
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
	}

	hr = m_pRenderer->GetDev()->CreateRootSignature(0, m_pRootBlob->GetBufferPointer(), m_pRootBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
	if (FAILED(hr))
	{
		assert(0 && "ルートシグネチャの作成に失敗。");
	}
}

void RootSignature::CreateRange(D3D12_DESCRIPTOR_RANGE& pRange, RangeType type, UINT count)
{
	switch (type)
	{
	case RangeType::CBV:
		pRange = {};
		pRange.NumDescriptors = 1;
		pRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		pRange.BaseShaderRegister = count;
		pRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;

	case RangeType::SRV:
		pRange = {};
		pRange.NumDescriptors = 1;
		pRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		pRange.BaseShaderRegister = count;
		pRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;

	case RangeType::UAV:
		pRange = {};
		pRange.NumDescriptors = 1;
		pRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		pRange.BaseShaderRegister = count;
		pRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;

	default:
		break;
	}
}

void RootSignature::CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC& pSamplerDesc, TextureAddressMode addressMode, D3D12Filter filter, UINT count)
{
	D3D12_TEXTURE_ADDRESS_MODE mode = addressMode == TextureAddressMode::Wrap ? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	D3D12_FILTER samplingFilter = filter == D3D12Filter::Point ? D3D12_FILTER_MIN_MAG_MIP_POINT : D3D12_FILTER_MIN_MAG_MIP_LINEAR;

	pSamplerDesc = {};
	pSamplerDesc.AddressU = mode;
	pSamplerDesc.AddressV = mode;
	pSamplerDesc.AddressW = mode;
	pSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	pSamplerDesc.Filter = samplingFilter;
	pSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	pSamplerDesc.MinLOD = 0.0f;
	pSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	pSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	pSamplerDesc.MaxAnisotropy = 16;
	pSamplerDesc.ShaderRegister = count;
}
