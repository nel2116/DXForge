#include "stdafx.h"
#include "Actor.h"

void Actor::BaseUpdate()
{
	// ��ɃR���|�[�l���g���X�V
	auto buff = m_pComponents;
	for (auto cmp : buff)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if (cmp->IsDead()) { m_pDeadComponents.push_back(cmp); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!cmp->IsActive()) { continue; }
		// �X�V
		cmp->Update();
	}
	// Actor�̍X�V
	Update();
}

void Actor::BaseDraw()
{
	// ��ɃR���|�[�l���g��`��
	auto buff = m_pComponents;
	for (auto cmp : buff)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if (cmp->IsDead()) { m_pDeadComponents.push_back(cmp); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!cmp->IsActive()) { continue; }
		// �`��
		cmp->Draw();
	}
	// Actor�̕`��
	Draw();

	// �j�����ꂽ�R���|�[�l���g���폜
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
	// Order�̏��Ԃŏ����ɕ��ׂ�
	std::sort(m_pComponents.begin(), m_pComponents.end(), [](Component* a, Component* b) { return a->GetOrder() < b->GetOrder(); });
}

void Actor::DestroyComponent()
{
	// �j�����ꂽ�R���|�[�l���g�����X�g����폜
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


