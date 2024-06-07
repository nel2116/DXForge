#include "stdafx.h"
#include "Actor.h"

void Actor::BaseUpdate()
{
	// 先にコンポーネントを更新
	auto buff = m_pComponents;
	for (auto cmp : buff)
	{
		// 破棄されている場合はリストに移動させて次へ
		if (cmp->IsDead()) { m_pDeadComponents.push_back(cmp); continue; }
		// 使われていない場合は次へ
		if (!cmp->IsActive()) { continue; }
		// 更新
		cmp->Update();
	}
	// Actorの更新
	Update();
}

void Actor::BaseDraw()
{
	// 先にコンポーネントを描画
	auto buff = m_pComponents;
	for (auto cmp : buff)
	{
		// 破棄されている場合はリストに移動させて次へ
		if (cmp->IsDead()) { m_pDeadComponents.push_back(cmp); continue; }
		// 使われていない場合は次へ
		if (!cmp->IsActive()) { continue; }
		// 描画
		cmp->Draw();
	}
	// Actorの描画
	Draw();

	// 破棄されたコンポーネントを削除
	if (m_pDeadComponents.size() != 0) { DestroyComponent(); SortComponent(); }
}

void Actor::BaseInit()
{
	Init();
}

void Actor::BaseUninit()
{
	for (Component* cmp : m_pComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pComponents.clear();

	Uninit();
}

void Actor::SortComponent()
{
	// Orderの順番で昇順に並べる
	std::sort(m_pComponents.begin(), m_pComponents.end(), [](Component* a, Component* b) { return a->GetOrder() < b->GetOrder(); });
}

void Actor::DestroyComponent()
{
	// 破棄されたコンポーネントをリストから削除
	for (auto cmp : m_pDeadComponents)
	{
		auto itr = std::find(m_pComponents.begin(), m_pComponents.end(), cmp);
		if (itr != m_pComponents.end())
		{
			m_pComponents.erase(itr);
		}
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pDeadComponents.clear();
}


