// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 制作者 : 田中ミノル
// 制作日 : 2024/06/04 16:20
// 概要   : Actorクラスのヘッダファイル
// 更新日 
// 2024/06/04 16:20 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <vector>
#include <Component/Component.h>

using namespace std;


// ====== クラスの定義 ======
class Actor
{
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Init() {}
	virtual void Uninit() {}

	void BaseUpdate();
	void BaseDraw();
	void BaseInit();
	void BaseUninit();



	// Actorにオブジェクトを追加
	template<class T>
	T* AddComponent();


private:
	vector<Component*> m_pComponents;	// コンポーネントリスト
	vector<Component*> m_pDeadComponents;	// 破棄されたコンポーネントのリスト
};

template<class T>
inline T* Actor::AddComponent()
{
	// コンポーネントを生成
	T* cmp = NEW T();
	// コンポーネントをリストに追加
	m_pComponents.emplace_back(cmp);
	cmp->SetParent(this);
	cmp->Init();
	return cmp;
}
