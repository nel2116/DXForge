// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Shader.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 18:00
// 概要   : シェーダークラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Pipeline/Pipeline.h"
#include "RootSignature/RootSignature.h"

struct RenderingSetting
{
	std::vector<InputLayout> inputLayouts;
	std::vector<DXGI_FORMAT> Formats;
	CullMode cullMode = CullMode::Back;
	BlendMode blendMode = BlendMode::Alpha;
	PrimitiveTopology primitiveTopology = PrimitiveTopology::Triangle;
	bool isDepth = true;
	bool isDepthMask = true;
	int RTVCount = 1;
	bool isWireFrame = false;
};

class Mesh;
class Shader
{
public:		// パブリック関数
	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="pDev">レンダーターゲットクラスのポインタ</param>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="setting">レンダリング設定</param>
	/// <param name="rangeType">レンジタイプ</param>
	void Create(Renderer* pDev, std::string& filePath, RenderingSetting& setting, std::vector<RangeType> rangeTypes);

	/// <summary>
	/// 描画開始
	/// </summary>	
	/// <param name="w">横幅</param>
	/// <param name="h">縦幅</param>
	void Begin(int w, int h);

	/// <summary>
	/// メッシュの描画
	/// </summary>
	/// <param name="mesh">メッシュ</param>
	void DrawMesh(const Mesh& mesh);

	/// <summary>
	/// CBVカウント取得
	/// </summary>
	/// <returns>CBVカウント</returns>
	UINT GetCBVCount() const { return m_cbvCount; }

private:		// プライベート関数
	/// <summary>
	/// シェーダーファイルのロード
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void LoadShader(const std::string& filePath);

private:		// メンバ変数
	Renderer* m_pRenderer = nullptr;

	// パイプライン
	std::unique_ptr<Pipeline> m_upPipeline = nullptr;
	std::unique_ptr<RootSignature> m_upRootSignature = nullptr;

	// シェーダー
	ID3DBlob* m_pVSBlob = nullptr;	// 頂点シェーダーバッファ
	ID3DBlob* m_pHSBlob = nullptr;	// ハルシェーダーバッファ
	ID3DBlob* m_pDSBlob = nullptr;	// ドメインシェーダーバッファ
	ID3DBlob* m_pGSBlob = nullptr;	// ジオメトリシェーダーバッファ
	ID3DBlob* m_pPSBlob = nullptr;	// ピクセルシェーダーバッファ

	UINT m_cbvCount = 0;	// CBVカウント
};