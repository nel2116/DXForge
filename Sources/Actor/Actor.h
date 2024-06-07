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
	Actor() : m_bIsDead(false), m_bIsActive(true), m_sTag("none"), m_nOrder(0) {}
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Init() {}
	virtual void Uninit() {}

	void BaseUpdate();
	void BaseDraw();
	void BaseInit();
	void BaseUninit();

public:	// アクセサ
	// Actorにオブジェクトを追加
	template<class T>
	T* AddComponent();
	// コンポーネントの取得
	template<class T>
	T* GetComponent();
	// コンポーネントをタグで指定して取得
	template<class T>
	T* GetComponent(const string& tag);

	// 破棄フラグを立てる
	void Destroy() { m_bIsDead = true; }
	// 死亡フラグの取得
	bool IsDead() { return m_bIsDead; }

	// アクティブフラグの設定
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	// アクティブフラグの取得
	bool IsActive() { return m_bIsActive; }

	// タグの設定
	void SetTag(const string& tag) { m_sTag = tag; }
	// タグの取得
	string GetTag() { return m_sTag; }

	// 更新順序の設定
	void SetOrder(int nOrder) { m_nOrder = nOrder; }
	// 更新順序の取得
	int GetOrder() { return m_nOrder; }

private:
	// コンポーネントリストをOrderの順番に昇順で整列
	void SortComponent();

	// コンポーネントの破棄
	void DestroyComponent();

private:
	vector<Component*> m_pComponents;		// コンポーネントリスト
	vector<Component*> m_pDeadComponents;	// 破棄されたコンポーネントのリスト
	bool m_bIsDead;							// 死亡フラグ : trueの場合、Actorは削除される
	bool m_bIsActive;						// アクティブフラグ : falseの場合、Actorは更新されない
	std::string m_sTag;						// タグ
	int m_nOrder;							// 更新順序
};

template<class T>
inline T* Actor::AddComponent()
{
	// コンポーネントの生成
	T* pCmp = NEW T();
	pCmp->SetOwner(this);
	// コンポーネントの追加
	m_pComponents.push_back(pCmp);
	// コンポーネントの初期化
	((Component*)pCmp)->Init();
	// コンポーネントの整列
	SortComponent();

	return pCmp;
}

template<class T>
inline T* Actor::GetComponent()
{
	for (Component* pCmp : m_pComponents)
	{
		if (typeid(*pCmp) == typeid(T))
		{
			return (T*)pCmp;
		}
	}
	return nullptr;
}

template<class T>
inline T* Actor::GetComponent(const string& tag)
{
	for (Component* pCmp : m_pComponents)
	{
		if (typeid(*pCmp) == typeid(T))
		{
			if (pCmp->GetTag() == tag)
			{
				return (T*)pCmp;
			}
		}
	}
	return nullptr;
}

