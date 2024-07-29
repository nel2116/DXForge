// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [MeshComponent.h]
// 制作者 : 田中ミノル
// 作成日 : 2024/07/02
// 概要   : メッシュコンポーネントの定義
// 更新日
// 2024/07/02 : 田中ミノル : 新規作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Component.h"
#include <System/MyMath.h>

struct alignas(256) CbMesh
{
	DirectX::SimpleMath::Matrix   World;      // ワールド行列
};

struct alignas(256) CbMaterial
{
	DirectX::SimpleMath::Vector3 BaseColor;  // 基本色
	float   Alpha;      // 透過度
	float   Roughness;  // 面の粗さ(範囲は0~1)
	float   Metallic;   // 金属度(範囲は[0~1])
};

// ====== クラスの定義 ======
class ModelComponent : public Component
{
public:	// パブリック関数
	// コンストラクタ
	ModelComponent(std::string tag = "") : Component(tag) {}
	// デストラクタ
	~ModelComponent();

	// 初期化
	void Init() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	// 終了処理
	void Uninit() override;

	// モデルの読み込み
	void LoadModel(std::string fileName);

private:
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// メッシュ用バッファ
	std::vector<Mesh*> m_pMesh;							// メッシュ
	Material m_Material;								// マテリアル
};




