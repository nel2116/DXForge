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

// ====== クラスの定義 ======
class MeshComponent : public Component
{
public:	// パブリック関数
	// コンストラクタ
	MeshComponent(std::string tag = "") : Component(tag) {}
	// デストラクタ
	~MeshComponent();

	// 初期化
	void Init() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	// 終了処理
	void Uninit() override;

private:
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];		// ライトバッファ
	ConstantBuffer m_CameraCB[FRAME_BUFFER_COUNT];		// カメラバッファ
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// 変換用バッファ
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// メッシュ用バッファ
	std::vector<Mesh*> m_pMesh;							// メッシュ
	Material m_Material;								// マテリアル

};




