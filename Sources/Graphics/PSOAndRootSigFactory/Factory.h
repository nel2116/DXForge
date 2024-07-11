// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Factory.h]
// 制作者 : 田中ミノル
// 作成日 : 2024/07/02
// 概要   : PSOとルートシグネチャの生成クラス
// 更新日
// 2024/07/02 : 田中ミノル : 新規作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Manager/Manager.h"
#include <System/ComPtr.h>

// ====== クラスの定義 ======
class Factory : public Manager<Factory>
{
public:	// パブリック関数

	/// <summary>
	/// パイプラインステートの生成
	/// </summary>
	/// <param name="name">生成するパイプラインステートの名前</param>
	/// <param name="pRootSig">ルートシグネチャ</param>
	/// <param name="pVS">頂点シェーダ</param>
	/// <param name="pPS">ピクセルシェーダ</param>
	/// <param name="pInputLayout">インプットレイアウト</param>
	/// <param name="topology">プリミティブトポロジ</param>
	/// <param name="depthStencilState">深度ステンシルステート</param>
	/// <param name="blendState">ブレンドステート</param>
	/// <param name="rasterizerState">ラスタライザステート</param>
	/// <param name="sampleMask">サンプルマスク</param>
	/// <param name="sampleDesc">サンプル設定</param>
	/// <param name="numRenderTargets">レンダーターゲットの数</param>
	/// <param name="rtvFormats">レンダーターゲットのフォーマット</param>
	/// <param name="dsvFormat">デプスステンシルビューフォーマット</param>
	///	<param name="depthStencilViewFlags">デプスステンシルビューフラグ</param>
	/// <returns>パイプラインステートのポインタ</returns>
	ComPtr<ID3D12PipelineState> CreatePSO(
		const std::string& name,
		const RootSignature* pRootSig,
		const ID3DBlob* pVS,
		const ID3DBlob* pPS,
		const D3D12_INPUT_ELEMENT_DESC* pElement,
		const D3D12_PRIMITIVE_TOPOLOGY_TYPE topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		const D3D12_DEPTH_STENCIL_DESC depthStencilState,
		const D3D12_BLEND_DESC blendState,
		const D3D12_RASTERIZER_DESC rasterizerState,
		const UINT sampleMask = UINT_MAX,
		const DXGI_SAMPLE_DESC sampleDesc,
		const UINT numRenderTargets = 1,
		const DXGI_FORMAT rtvFormat,
		const DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT,
		const D3D12_DSV_FLAGS depthStencilViewFlags = D3D12_DSV_FLAG_NONE
	);

	/// <summary>
	/// ルートシグネチャの設定
	/// </summary>
	/// <param name="name">ルートシグネチャの名前</param>
	/// <param name="pRootSig">ルートシグネチャ</param>
	void SetRootSignature(const std::string& name, RootSignature* pRootSig);

	/// <summary>
	/// ルートシグネチャの取得
	/// </summary>
	/// <param name="name">ルートシグネチャの名前</param>
	/// <returns>ルートシグネチャのポインタ</returns>
	RootSignature* GetRootSignature(const std::string& name);

	/// <summary>
	/// パイプラインステートの取得
	/// </summary>
	/// <param name="name">パイプラインステートの名前</param>
	/// <returns>パイプラインステートのポインタ</returns>
	ID3D12PipelineState* GetPSO(const std::string& name);

private:
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> m_pPSO;	// パイプラインステート
	std::unordered_map<std::string, RootSignature*> m_pRootSig;				// ルートシグニチャ

private:	// シングルトン関連
	Factory() = default;
	friend class Manager<Factory>;
};