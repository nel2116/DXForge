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
	std::vector<Actor*> m_pActors;	// Actor���X�g
	std::vector<Actor*> m_pDeadActors;	// �j�����ꂽActor���X�g


private:	// �V���O���g���֘A
	friend class Manager<ActorManager>;
	ActorManager() = default;

	// �f�t�H���g�R���X�g���N�^�ȊO�ŃC���X�^���X���\�z�������ꍇ�AcreateInstance()���㏑������
	// ��
	// Hoge(int) { std::cout << "Hoge(int)" << std::endl; }
	// static Hoge *createInstance()
	// {
	//   return NEW Hoge(0);
	// }
};

// �C���X�^���X�̎擾
#define ACTOR_MANAGER ActorManager::manager()

template<class T>
inline T* ActorManager::AddActor()
{
	// Actor�̒ǉ�
	T* buff = NEW T();
	m_pActors.push_back(buff);
	buff->BaseInit();
	return buff;
}

template<class T>
inline T* ActorManager::GetActor()
{
	// Actor�̎擾
	for (Actor* pActor : m_pActors)
	{
		if (typeid(*pActor) == typeid(T))
		{
			return (T*)pActor;
		}
	}
	return nullptr;
}

template<class T>
inline T* ActorManager::GetActor(const string& tag)
{
	// Actor�̎擾
	for (Actor* pActor : m_pActors)
	{
		if (typeid(*pActor) == typeid(T))
		{
			if (pActor->GetTag() == tag)
			{
				return (T*)pActor;
			}
		}
	}
	return nullptr;
}
