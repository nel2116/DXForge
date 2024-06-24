// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RootSignature.h]
// 作成者 : 田中ミノル
// 作成日 : 20124/06/24
// 概要   : ルートシグネチャクラスの定義
// 更新履歴
// 2024/06/24  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/ComPtr.h>
#include <d3d12.h>
#include <vector>

// ====== 列挙体 ======
enum ShaderStage
{
	ALL = 0, // 全ステージ
	VS = 1,  // 頂点シェーダ
	HS = 2,  // ハルシェーダ
	DS = 3,  // ドメインシェーダ
	GS = 4,  // ジオメトリシェーダ
	PS = 5,  // ピクセルシェーダ
};

enum SamplerState
{
	PointWrap = 0,		// ポイントサンプリング、ラップ
	PointClamp = 1,		// ポイントサンプリング、クランプ
	LinearWrap = 2,		// リニアサンプリング、ラップ
	LinearClamp = 3,	// リニアサンプリング、クランプ
	AnisotropicWrap = 4,// 異方性フィルタリング、ラップ
	AnisotropicClamp = 5,// 異方性フィルタリング、クランプ
};

// ====== クラス定義 ======
class RootSignature
{
public:
	// ディスククラス
	class Desc
	{
	public:
		Desc();
		~Desc();
		Desc& Begin(int count);
		Desc& SetCBV(ShaderStage stage, int index, uint32_t reg);
		Desc& SetSRV(ShaderStage stage, int index, uint32_t reg);
		Desc& SetUAV(ShaderStage stage, int index, uint32_t reg);
		Desc& SetSmp(ShaderStage stage, int index, uint32_t reg);
		Desc& AddStaticSmp(ShaderStage stage, uint32_t reg, SamplerState state);
		Desc& AllowIL();
		Desc& AllowSO();
		Desc& End();
		const D3D12_ROOT_SIGNATURE_DESC* GetDesc() const;
	private:
		std::vector<D3D12_DESCRIPTOR_RANGE>     m_Ranges;
		std::vector<D3D12_STATIC_SAMPLER_DESC>  m_Samplers;
		std::vector<D3D12_ROOT_PARAMETER>       m_Params;
		D3D12_ROOT_SIGNATURE_DESC               m_Desc;
		bool                                    m_DenyStage[5];
		uint32_t                                m_Flags;

		void CheckStage(ShaderStage stage);
		void SetParam(ShaderStage, int index, uint32_t reg, D3D12_DESCRIPTOR_RANGE_TYPE type);
	};

	// パブリック関数
	RootSignature();
	~RootSignature();
	bool Init(const D3D12_ROOT_SIGNATURE_DESC* pDesc);
	void Uninit();
	ID3D12RootSignature* GetPtr() const;

private:
	// メンバ変数
	ComPtr<ID3D12RootSignature> m_pRootSignature;
};
