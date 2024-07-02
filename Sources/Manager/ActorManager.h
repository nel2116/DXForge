// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ����� : �c���~�m��
// ����� : 2024/06/06 18:00
// �T�v   : ActorManager�N���X�̃w�b�_�t�@�C��	:	Actor�N���X�̊Ǘ��N���X
// �X�V�� 
// 2024/06/06 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Manager.h"
#include <Actor/Actor.h>

// ====== �N���X�̒�` ======
class ActorManager : public Manager<ActorManager>
{

public:	// �����o�֐�

	// �X�V
	void Update();
	// �`��
	void Draw();
	// ������
	void Init();
	// �I��
	void Uninit();

public:	// �A�N�Z�T

	// Actor�̒ǉ�
	template<class T>
	T* AddActor();
	// Actor�̎擾
	template<class T>
	T* GetActor();
	// Actor���^�O�Ŏw�肵�Ď擾
	template<class T>
	T* GetActor(const string& tag);

private:	// �v���C�x�[�g�֐�
	// Actor�̔j��
	void DestroyActor();

	// Actor�̃��X�g��Order�ŏ����ɕ��ׂ�
	void SortActor();

private:	// �����o�ϐ�
	std::vector<Actor*> m_pActors;		// Actor���X�g
	std::vector<Actor*> m_pTempActors;	// �ꎞ�I��Actor���X�g
	std::vector<Actor*> m_pDeadActors;	// �j�����ꂽActor���X�g
	bool m_bLooping;					// ���[�v�t���O

private:	// �V���O���g���֘A
	friend class Manager<ActorManager>;
	ActorManager()
		: m_bLooping(false)
	{}

	static ActorManager* createInstance()
	{
		return NEW ActorManager();
	}

};

// �C���X�^���X�̎擾
#define ACTOR_MANAGER ActorManager::manager()

template<class T>
inline T* ActorManager::AddActor()
{
	// Actor�̐���
	T* buff = NEW T();

	// �C�e���[�^�����삳��Ȃ��悤�A���[�v���͈ꎞ�I�ȃ��X�g�ɒǉ�����
	if (!m_bLooping)
	{
		// Actor�̒ǉ�
		m_pActors.push_back(buff);
		// Actor�̕��ёւ�
		SortActor();
	}
	else
	{
		// �ꎞ�I�ȃ��X�g�ɒǉ�
		m_pTempActors.push_back(buff);
	}
	// ������
	buff->BaseInit();

	return buff;
}

template<class T>
inline T* ActorManager::GetActor()
{
	// Actor�̎擾
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		if (typeid(*(*pActor)) == typeid(T))
		{
			return (T*)(*pActor);
		}
	}
	// ����Έꎞ�I�ȃ��X�g������T��
	if (m_pTempActors.size() != 0)
	{
		auto temp = m_pTempActors;
		for (auto pActor = temp.begin(), end = temp.end(); pActor != end; ++pActor)
		{
			if (typeid(*(*pActor)) == typeid(T))
			{
				return (T*)(*pActor);
			}
		}
	}

	return nullptr;
}

template<class T>
inline T* ActorManager::GetActor(const string& tag)
{
	// Actor�̎擾
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		if (typeid(*(*pActor)) == typeid(T) && (*pActor)->GetTag() == tag)
		{
			return (T*)(*pActor);
		}
	}

	// ����Έꎞ�I�ȃ��X�g������T��
	if (m_pTempActors.size() != 0)
	{
		auto temp = m_pTempActors;
		for (auto pActor = temp.begin(), end = temp.end(); pActor != end; ++pActor)
		{
			if (typeid(*(*pActor)) == typeid(T) && (*pActor)->GetTag() == tag)
			{
				return (T*)(*pActor);
			}
		}
	}

	return nullptr;
}
