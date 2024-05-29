// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Shader.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 20:00
// 概要   : シェーダークラスの実装
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Shader.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "../Mesh/Mesh.h"

// ====== 関数 ======
namespace
{
	std::wstring StringToWString(const std::string& str)
	{
		//呼び出し1回目(文字列数を得る)
		auto num1 = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), -1, nullptr, 0);

		std::wstring wstr;//stringのwchar_t版
		wstr.resize(num1);//得られた文字列数でリサイズ

		//呼び出し2回目(確保済みのwstrに変換文字列をコピー)
		auto num2 = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), -1, &wstr[0], num1);

		assert(num1 == num2);//一応チェック
		return wstr;

	}
}

// ====== メンバ関数 ======
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

	// ルートシグネチャのセット
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

	// ビューポートの設定
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

	// 頂点シェーダーのコンパイル
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
			assert(0 && "頂点シェーダーのコンパイルに失敗しました");
			OutputDebugStringA((char*)error->GetBufferPointer());
			error->Release();
			return;
		}
	}

	// ハルシェーダーのコンパイル
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
		// ハルシェーダーはなくても良い
	}

	// ドメインシェーダーのコンパイル
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
		// ドメインシェーダーはなくても良い
	}

	// ジオメトリシェーダーのコンパイル
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
		// ジオメトリシェーダーはなくても良い
	}

	// ピクセルシェーダーのコンパイル
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
			assert(0 && "ピクセルシェーダーのコンパイルに失敗しました");
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
