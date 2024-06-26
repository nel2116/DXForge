// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [TestScene.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/26 12:00
// 概要   : TestSceneクラスの定義	:	テストシーンクラス
// 更新日
// 2024/06/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Scene.h"

// ====== クラスの定義 ======
class TestScene : public Scene
{
public:	// メンバ関数
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 初期化
	void Init() override;
	// 終了
	void Uninit() override;

private:	// メンバ変数

};

