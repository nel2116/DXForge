#include "stdafx.h"
#include "ActorManager.h"

void ActorManager::Update()
{
	for (auto pActor : m_pActors)
	{
		// 破棄されている場合はリストに移動させて次へ
		if (pActor->IsDead()) { m_pDeadActors.push_back(pActor); continue; }
		// 使われていない場合は次へ
		if (!pActor->IsActive()) { continue; }
		pActor->BaseUpdate();
	}
}

void ActorManager::Draw()
{
	for (auto pActor : m_pActors)
	{
		// 破棄されている場合はリストに移動させて次へ
		if (pActor->IsDead()) { m_pDeadActors.push_back(pActor); continue; }
		// 使われていない場合は次へ
		if (!pActor->IsActive()) { continue; }
		pActor->BaseDraw();
	}

	// 破棄されたActorを削除
	if (m_pDeadActors.size() != 0) { DestroyActor(); SortActor(); }
}

void ActorManager::Init()
{

}

void ActorManager::Uninit()
{
	for (auto pActor : m_pActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pActors.clear();
}

void ActorManager::DestroyActor()
{
	// 破棄されたコンポーネントをリストから削除
	for (auto pActor : m_pDeadActors)
	{
		auto itr = std::find(m_pActors.begin(), m_pActors.end(), pActor);
		if (itr != m_pActors.end())
		{
			m_pActors.erase(itr);
		}
		pActor->Uninit();
		SAFE_DELETE(pActor);
	}
	m_pDeadActors.clear();
}

void ActorManager::SortActor()
{
	// Orderの順番で昇順に並べる
	std::sort(m_pActors.begin(), m_pActors.end(), [](Actor* a, Actor* b) { return a->GetOrder() < b->GetOrder(); });
}
