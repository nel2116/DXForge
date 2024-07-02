#include "stdafx.h"
#include "Actor.h"

void Actor::BaseUpdate()
{
	// ���[�v���͈ꎞ�I�ȃ��X�g�ɒǉ�
	m_bLooping = true;
	// ��ɃR���|�[�l���g���X�V
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if ((*cmp)->IsDead()) { m_pDeadComponents.push_back(*cmp); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!(*cmp)->IsActive()) { continue; }
		// �X�V
		(*cmp)->Update();
	}
	m_bLooping = false;

	// �ꎞ�I�ȃ��X�g�̃R���|�[�l���g�������
	if (m_pTempComponents.size() != 0)
	{
		// �ꎞ�I�ȃ��X�g�̃R���|�[�l���g��{���X�g�Ɉړ�
		auto temp = m_pTempComponents;
		for (auto cmp = temp.begin(), end = temp.end(); cmp != end; ++cmp)
		{
			m_pComponents.push_back(*cmp);
		}
		// �R���|�[�l���g�̕��ёւ�
		SortComponent();
		// �ꎞ�I�ȃ��X�g���N���A
		m_pTempComponents.clear();
	}

	// ���g�̍X�V
	Update();
}

void Actor::BaseDraw()
{
	// ���[�v���͈ꎞ�I�ȃ��X�g�ɒǉ�
	m_bLooping = true;
	// ��ɃR���|�[�l���g��`��
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		// �j������Ă���ꍇ�̓��X�g�Ɉړ������Ď���
		if ((*cmp)->IsDead()) { m_pDeadComponents.push_back(*cmp); continue; }
		// �g���Ă��Ȃ��ꍇ�͎���
		if (!(*cmp)->IsActive()) { continue; }
		// �`��
		(*cmp)->Draw();
	}
	m_bLooping = false;

	// ���g�̕`��
	Draw();

	// �j���t���O�̂������R���|�[�l���g������΍폜
	if (m_pDeadComponents.size() != 0) { DestroyComponent(); SortComponent(); }
}

void Actor::BaseInit()
{
	Init();
}

void Actor::BaseUninit()
{
	// �R���|�[�l���g�̔j��
	for (Component* cmp : m_pComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pComponents.clear();

	// �ꎞ�I�ȃ��X�g���j��
	for (Component* cmp : m_pTempComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pTempComponents.clear();

	// �j���t���O�̂������R���|�[�l���g���j��
	for (Component* cmp : m_pDeadComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pDeadComponents.clear();

	// ���g�̏I������
	Uninit();
}

void Actor::SortComponent()
{
	// Order�̏��Ԃŏ����ɕ��ׂ�
	std::sort(m_pComponents.begin(), m_pComponents.end(), [](Component* a, Component* b) { return a->GetOrder() < b->GetOrder(); });
}

void Actor::DestroyComponent()
{
	// �j���t���O�̂������R���|�[�l���g���폜
	auto buff = m_pDeadComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		// �R���|�[�l���g�z�񂩂���폜
		auto itr = std::find(m_pComponents.begin(), m_pComponents.end(), (*cmp));
		if (itr != m_pComponents.end())
		{
			m_pComponents.erase(itr);
		}
		(*cmp)->Uninit();
		SAFE_DELETE(*cmp);
	}
	m_pDeadComponents.clear();
}


