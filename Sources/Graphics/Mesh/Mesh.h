// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 11:55 : 作成
// 概要   : メッシュクラスの定義
// 更新履歴
// 2024/06/14  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "ResMesh.h"
#include <Graphics/Buffer/VertexBuffer/VertexBuffer.h>
#include <Graphics/Buffer/IndexBuffer/IndexBuffer.h>

// ====== クラス定義 ======
class Mesh
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Mesh();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Mesh();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="resource">リソースメッシュ</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(const ResMesh& resource);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// マテリアルIDを取得
	/// </summary>
	uint32_t GetMaterialId() const;

private:	// メンバ変数
	VertexBuffer    m_VB;               //!< 頂点バッファです.
	IndexBuffer     m_IB;               //!< インデックスバッファです.
	uint32_t        m_MaterialId;       //!< マテリアルIDです.
	uint32_t        m_IndexCount;       //!< インデックス数です.

private:	// コピー禁止
	Mesh(const Mesh&) = delete;     // アクセス禁止.
	void operator = (const Mesh&) = delete;     // アクセス禁止.
};