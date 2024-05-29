// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : メッシュクラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <string>
#include <memory>
#include <Graphics/Renderer.h>
#include <Graphics/Texture/Texture.h>
#include "MeshData/MeshData.h"

// ====== 名前空間 ======
using namespace DirectX;

// ====== 構造体定義 ======
struct MeshFace
{
	UINT idx[3];
};

struct Material
{
	std::string name;									// マテリアルの名前
	std::shared_ptr<Texture> spBaseColorTex;			// テクスチャ
	XMFLOAT4 baseColor = { 1.0f,1.0f,1.0f,1.0f };		// ベースカラー
	std::shared_ptr<Texture> spMetallicRoughnessTex;	// R:メタリック G:ラフネス
	float metallic = 0.0f;								// メタリック
	float roughness = 0.0f;								// ラフネス
	std::shared_ptr<Texture> spEmissiveTex;				// 自己発光テクスチャ
	XMFLOAT3 emissive = { 0.0f,0.0f,0.0f };				// 自己発光のスケーリング係数
	std::shared_ptr<Texture> spNormalTex;				// 法線テクスチャ
};

// ====== クラス定義 ======
class Mesh
{
private:	// 型定義
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	/// <param name="vertices">頂点情報</param>
	/// <param name="faces">面情報</param>
	/// <param name="materials">マテリアル情報</param>
	void Create(Renderer* pDev, const std::vector<MeshVertex>& vertices, const std::vector<MeshFace>& faces, const Material& material);

	/// <summary>
	/// インスタンス描画
	/// </summary>
	/// <param name="vertexCount">頂点数</param>
	void DrawInstanced(UINT vertexCount) const;

	/// <summary>
	/// インスタンス数を取得
	/// </summary>
	/// <returns>インスタンス数</returns>
	UINT GetInstanceCount() const { return m_instanceCount; }

	/// <summary>
	/// マテリアルの取得
	/// </summary>
	/// <returns>マテリアル情報</returns>
	const Material& GetMaterial() const { return m_material; }

private:
	Renderer* m_pRenderer = nullptr;
	ComPtr<ID3D12Resource> m_pVertexBuff = nullptr;
	ComPtr<ID3D12Resource> m_pIndexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};
	UINT m_instanceCount = 0;
	Material m_material = {};
};