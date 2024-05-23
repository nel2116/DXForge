// [PMDRenderer.h]
// 作成日 : 2024/05/23 18:00
// 作成者 : 田中ミノル
// 概要   : PMDレンダラークラスの定義
#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>

class Dx12Wrapper;
class PMDActor;
class PMDRenderer
{
	friend PMDActor;

public:	// パブリック関数
	PMDRenderer(Dx12Wrapper& dx12);
	~PMDRenderer();
	void Update();
	void Draw();
	ID3D12PipelineState* GetPipelineState();
	ID3D12RootSignature* GetRootSignature();

private:	// プライベート関数
	ID3D12Resource* CreateDeaultTexture(size_t width, size_t height);
	ID3D12Resource* CreateWhiteTexture();	// 白テクスチャの生成
	ID3D12Resource* CreateBlackTexture();	// 黒テクスチャの生成
	ID3D12Resource* CreateGradationTexture();	// グレイスケールグラデーションテクスチャの生成

	// パイプライン初期化
	HRESULT CreateGraphicsPipelineForPMD();
	// ルートシグネチャ初期化
	HRESULT CreateRootSignature();

	bool CheckShaderCompileResult(HRESULT hr, ID3DBlob* err = nullptr);

private:	// メンバ変数
	Dx12Wrapper& m_dx12;
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12PipelineState> m_pipeline = nullptr;	// PMD用のパイプライン
	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;	// ルートシグネチャ

	// PMD用共通テクスチャ(白、黒、グレイスケールグラデーション)
	ComPtr<ID3D12Resource> m_whiteTex = nullptr;
	ComPtr<ID3D12Resource> m_blackTex = nullptr;
	ComPtr<ID3D12Resource> m_gradTex = nullptr;

};