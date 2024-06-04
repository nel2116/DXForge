#include "stdafx.h"
#include "Actor.h"

void Actor::BaseUpdate()
{
	// 先にコンポーネントを更新
	auto buff = m_pComponents;
	for (auto cmp : buff)
	{
		// 使われていない場合は次へ
		if (!cmp->IsActive()) { continue; }
		// 破棄されている場合はリストに移動させて次へ
		if (cmp->IsDead()) { continue; }
		// 更新
		cmp->Update();
	}
	// Actorの更新
	Update();
}

void Actor::BaseDraw()
{
}

void Actor::BaseInit()
{
}

void Actor::BaseUninit()
{
}
