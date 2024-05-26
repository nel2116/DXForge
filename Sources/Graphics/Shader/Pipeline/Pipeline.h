// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Pipeline.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 18:00
// 概要   : パイプラインクラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics/Renderer.h>

enum class CullMode
{
	None = D3D12_CULL_MODE_NONE,
	Front = D3D12_CULL_MODE_FRONT,
	Back = D3D12_CULL_MODE_BACK,
};

enum class BlendMode
{
	Add,
	Alpha,
};

enum class InputLayout
{
	POSITION,
	TEXCOORD,
	NORMAL,
	TANGENT,
	COLOR,
	SKININDEX,
	SKINWEIGHT,
};

enum class PrimitiveTopology
{
	Undefined = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
	Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
	Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
	Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
	Patch = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
};

// ====== クラスの定義 ======
class RootSignature;
class Pipeline
{
private:	// 型定義
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// 描画設定のセット
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	/// <param name="pRootSignature">ルートシグネチャクラスのポインタ</param>
	/// <param name="inputLayout">入力レイアウト情報</param>
	/// <param name="cullMode">カリングモード</param>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="topology">プリミティブトポロジー</param>
	void SetRenderSetting(Renderer* pDev, RootSignature* pRootSignature, std::vector<InputLayout>& inputLayouts, CullMode cullMode, BlendMode blendMode, PrimitiveTopology topology);

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="pBlob">シェーダーデータリスト</param>
	/// <param name="formats">フォーマットリスト</param>
	/// <param name="isDepth">深度情報テスト</param>
	/// <param name="isDepthMask">深度書き込み</param>
	/// <param name="rtvCount">RTVの数</param>
	/// <param name="isWireFrame">ワイヤーフレームかどうか</param>
	void Create(std::vector<ID3DBlob*> pBlob, const std::vector<DXGI_FORMAT>formats, bool isDepth, bool isDepthMask, int rtvCount, bool isWireFrame);

	/// <summary>
	/// パイプラインの取得
	/// </summary>
	/// <returns>パイプラインのポインタ</returns>
	ID3D12PipelineState* GetPipeline() { return m_pPipelineState.Get(); }

	/// <summary>
	/// トポロジータイプの取得
	/// </summary>
	/// <returns>トポロジータイプ</returns>
	PrimitiveTopology GetTopology() { return m_topologyType; }

private:		// プライベート関数
	void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElements, std::vector<InputLayout>& inputLayouts);

	/// <summary>
	/// ブレンドモードのセット
	/// </summary>
	/// <param name="blendDesc">レンダーテーゲットブレンド情報</param>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& blendDesc, BlendMode blendMode);

private:		// メンバ変数
	Renderer* m_pRenderer = nullptr;
	RootSignature* m_pRootSignature = nullptr;

	std::vector<InputLayout> m_inputLayouts;
	CullMode m_cullMode;
	BlendMode m_blendMode;
	PrimitiveTopology m_topologyType;

	ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
};
