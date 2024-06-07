#include "stdafx.h"
#include "ActorManager.h"

void ActorManager::Update()
{
	for (auto pActor : m_pActors)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if (pActor->IsDead()) { m_pDeadActors.push_back(pActor); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!pActor->IsActive()) { continue; }
		pActor->BaseUpdate();
	}
}

void ActorManager::Draw()
{
	for (auto pActor : m_pActors)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if (pActor->IsDead()) { m_pDeadActors.push_back(pActor); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!pActor->IsActive()) { continue; }
		pActor->BaseDraw();
	}

	// �j�����ꂽActor���폜
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
	// �j�����ꂽ�R���|�[�l���g�����X�g����폜
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
	// Order�̏��Ԃŏ����ɕ��ׂ�
	std::sort(m_pActors.begin(), m_pActors.end(), [](Actor* a, Actor* b) { return a->GetOrder() < b->GetOrder(); });
}
