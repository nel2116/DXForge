#include "stdafx.h"
#include "ActorManager.h"

// Actorの更新
void ActorManager::Update()
{
	// ループ中はActorの追加ができないようにする
	m_bLooping = true;
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		// 破棄されている場合はリストに移動させて次へ
		if ((*pActor)->IsDestroy()) { m_pDeadActors.push_back(*pActor); continue; }
		// 使われていない場合は次へ
		if (!(*pActor)->IsActive()) { continue; }
		(*pActor)->BaseUpdate();
	}
	m_bLooping = false;

	// 一時的なリストのActorがあれば
	if (m_pTempActors.size() != 0)
	{
		auto temp = m_pTempActors;
		// 一時的なリストのActorを本リストに移動
		for (auto pActor = temp.begin(), end = temp.end(); pActor != end; ++pActor)
		{
			m_pActors.push_back(*pActor);
		}
		// Actorの並び替え
		SortActor();
		// 一時的なリストをクリア
		m_pTempActors.clear();
	}
}

// Actorの描画
void ActorManager::Draw()
{
	// ループ中はActorの追加ができないようにする
	m_bLooping = true;
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		// 破棄されている場合はリストに移動させて次へ
		if ((*pActor)->IsDestroy()) { m_pDeadActors.push_back(*pActor); continue; }
		// 使われていない場合は次へ
		if (!(*pActor)->IsActive()) { continue; }
		(*pActor)->BaseDraw();
	}
	m_bLooping = false;

	// 破棄フラグのたったActorを削除
	if (m_pDeadActors.size() != 0) { DestroyActor(); SortActor(); }
}

void ActorManager::Init()
{

}

void ActorManager::Uninit()
{
	// Actorの破棄
	for (auto pActor : m_pActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pActors.clear();
	// 一時的なリストのActorの破棄
	for (auto pActor : m_pTempActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pTempActors.clear();
	// 破棄されたActorの破棄
	for (auto pActor : m_pDeadActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pDeadActors.clear();
}

void ActorManager::DestroyActor()
{
	// 破棄フラグのたったActorを削除
	auto buff = m_pDeadActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		// Actor配列からも削除
		auto itr = std::find(m_pActors.begin(), m_pActors.end(), (*pActor));
		if (itr != m_pActors.end())
		{
			m_pActors.erase(itr);
		}
		(*pActor)->BaseUninit();
		SAFE_DELETE(*pActor);
	}
	m_pDeadActors.clear();
}

void ActorManager::SortActor()
{
	// Orderの順番で昇順に並べる
	auto buff = m_pActors;
	std::sort(buff.begin(), buff.end(), [](Actor* a, Actor* b) { return a->GetOrder() < b->GetOrder(); });
}
