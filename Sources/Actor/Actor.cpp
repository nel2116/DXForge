#include "stdafx.h"
#include "Actor.h"

void Actor::BaseUpdate()
{
	// ��ɃR���|�[�l���g���X�V
	auto buff = m_pComponents;
	for (auto cmp : buff)
	{
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!cmp->IsActive()) { continue; }
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if (cmp->IsDead()) { continue; }
		// �X�V
		cmp->Update();
	}
	// Actor�̍X�V
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
