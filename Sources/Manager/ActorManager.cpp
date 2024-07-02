#include "stdafx.h"
#include "ActorManager.h"

// Actor�̍X�V
void ActorManager::Update()
{
	// ���[�v����Actor�̒ǉ����ł��Ȃ��悤�ɂ���
	m_bLooping = true;
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if ((*pActor)->IsDestroy()) { m_pDeadActors.push_back(*pActor); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!(*pActor)->IsActive()) { continue; }
		(*pActor)->BaseUpdate();
	}
	m_bLooping = false;

	// �ꎞ�I�ȃ��X�g��Actor�������
	if (m_pTempActors.size() != 0)
	{
		auto temp = m_pTempActors;
		// �ꎞ�I�ȃ��X�g��Actor��{���X�g�Ɉړ�
		for (auto pActor = temp.begin(), end = temp.end(); pActor != end; ++pActor)
		{
			m_pActors.push_back(*pActor);
		}
		// Actor�̕��ёւ�
		SortActor();
		// �ꎞ�I�ȃ��X�g���N���A
		m_pTempActors.clear();
	}
}

// Actor�̕`��
void ActorManager::Draw()
{
	// ���[�v����Actor�̒ǉ����ł��Ȃ��悤�ɂ���
	m_bLooping = true;
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if ((*pActor)->IsDestroy()) { m_pDeadActors.push_back(*pActor); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!(*pActor)->IsActive()) { continue; }
		(*pActor)->BaseDraw();
	}
	m_bLooping = false;

	// �j���t���O�̂�����Actor���폜
	if (m_pDeadActors.size() != 0) { DestroyActor(); SortActor(); }
}

void ActorManager::Init()
{

}

void ActorManager::Uninit()
{
	// Actor�̔j��
	for (auto pActor : m_pActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pActors.clear();
	// �ꎞ�I�ȃ��X�g��Actor�̔j��
	for (auto pActor : m_pTempActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pTempActors.clear();
	// �j�����ꂽActor�̔j��
	for (auto pActor : m_pDeadActors)
	{
		pActor->BaseUninit();
		SAFE_DELETE(pActor);
	}
	m_pDeadActors.clear();
}

void ActorManager::DestroyActor()
{
	// �j���t���O�̂�����Actor���폜
	auto buff = m_pDeadActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		// Actor�z�񂩂���폜
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
	// Order�̏��Ԃŏ����ɕ��ׂ�
	auto buff = m_pActors;
	std::sort(buff.begin(), buff.end(), [](Actor* a, Actor* b) { return a->GetOrder() < b->GetOrder(); });
}
