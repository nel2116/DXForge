// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RootSignature.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 18:00
// 概要   : ルートシグネチャークラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics/Renderer.h>

enum class RangeType
{
	CBV,
	SRV,
	UAV,
	CONSTANT,
	DESCRIPTOR_TABLE,
	SAMPLER,
};

enum class TextureAddressMode
{
	Wrap,
	Mirror,
	Clamp,
	Border,
	MirrorOnce,
};

enum class D3D12Filter
{
	Point,
	Linear,
};

class RootSignature
{
private:	// 型定義
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// メンバ関数
	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	/// <param name="rangeType">レンジタイプリスト</param>
	void Create(Renderer* pDev, std::vector<RangeType>& rangeType);

	/// <summary>
	/// ルートシグネチャーの取得
	/// </summary>
	/// <returns>ルートシグネチャーのポインタ</returns>
	ID3D12RootSignature* GetRootSignature() { return m_pRootSignature.Get(); }

private:	// プライベート関数
	/// <summary>
	/// レンジの作成
	/// </summary>
	/// <param name="pRange">レンジのポインタ</param>
	/// <param name="type">レンジタイプ</param>
	/// <param name="count">登録数</param>
	void CreateRange(D3D12_DESCRIPTOR_RANGE& pRange, RangeType type, UINT count);

	/// <summary>
	/// サンプラーの作成
	/// </summary>
	/// <param name="pSamplerDesc">サンプラーデスクのポインタ</param>
	/// <param name="addressMode">アドレスモード</param>
	/// <param name="filter">フィルタ</param>
	/// <param name="count">使用サンプラー数</param>
	void CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC& pSamplerDesc, TextureAddressMode addressMode, D3D12Filter filter, UINT count);


private:	// メンバ変数
	Renderer* m_pRenderer = nullptr;
	ComPtr<ID3DBlob> m_pRootBlob = nullptr;
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;
};
