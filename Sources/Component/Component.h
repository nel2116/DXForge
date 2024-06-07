// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 制作者 : 田中ミノル
// 制作日 : 2024/06/04 16:20
// 概要   : Componentクラスのヘッダファイル
// 更新日 
// 2024/06/04 16:20 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <string>

// ====== クラスの定義 ======
class Actor;
class Component
{
public:	// メンバ関数
	Component() : m_pOwner(nullptr), m_bIsActive(true), m_bIsDead(false), m_nOrder(0), m_sTag("none") {}
	virtual ~Component() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Init() = 0;
	virtual void Uninit() = 0;

	void Destroy() { m_bIsDead = true; }

public:	// アクセサ
	void SetOwner(Actor* parent) { m_pOwner = parent; }
	Actor* GetOwner() { return m_pOwner; }
	void SetOrder(int nOrder) { m_nOrder = nOrder; }
	int GetOrder() { return m_nOrder; }
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	bool IsActive() { return m_bIsActive; }
	bool IsDead() { return m_bIsDead; }
	void SetTag(const std::string& tag) { m_sTag = tag; }
	std::string GetTag() { return m_sTag; }

protected:
	Actor* m_pOwner;	// 親オブジェクト

private:
	bool m_bIsActive;	// アクティブフラグ : falseの場合、コンポーネントは更新されない
	bool m_bIsDead;		// 死亡フラグ : trueの場合、コンポーネントは削除される
	int m_nOrder;		// 更新順序
	std::string m_sTag;	// タグ
};