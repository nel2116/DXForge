// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [TestActor.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/26 12:00
// 概要   : TestActorクラスの定義	:	テストアクタークラス
// 更新日
// 2024/06/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Actor.h"

// ====== クラスの定義 ======
class TestActor : public Actor
{
public:	// メンバ関数
	// 初期化
	void Init() override;
	// 終了
	void Uninit() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;

private:	// メンバ変数

};


