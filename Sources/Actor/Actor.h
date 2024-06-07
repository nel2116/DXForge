// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ����� : �c���~�m��
// ����� : 2024/06/04 16:20
// �T�v   : Actor�N���X�̃w�b�_�t�@�C��
// �X�V�� 
// 2024/06/04 16:20 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <vector>
#include <Component/Component.h>

using namespace std;


// ====== �N���X�̒�` ======
class Actor
{
public:
	Actor() : m_bIsDead(false), m_bIsActive(true), m_sTag("none"), m_nOrder(0) {}
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Init() {}
	virtual void Uninit() {}

	void BaseUpdate();
	void BaseDraw();
	void BaseInit();
	void BaseUninit();

public:	// �A�N�Z�T
	// Actor�ɃI�u�W�F�N�g��ǉ�
	template<class T>
	T* AddComponent();
	// �R���|�[�l���g�̎擾
	template<class T>
	T* GetComponent();
	// �R���|�[�l���g���^�O�Ŏw�肵�Ď擾
	template<class T>
	T* GetComponent(const string& tag);

	// �j���t���O�𗧂Ă�
	void Destroy() { m_bIsDead = true; }
	// ���S�t���O�̎擾
	bool IsDead() { return m_bIsDead; }

	// �A�N�e�B�u�t���O�̐ݒ�
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	// �A�N�e�B�u�t���O�̎擾
	bool IsActive() { return m_bIsActive; }

	// �^�O�̐ݒ�
	void SetTag(const string& tag) { m_sTag = tag; }
	// �^�O�̎擾
	string GetTag() { return m_sTag; }

	// �X�V�����̐ݒ�
	void SetOrder(int nOrder) { m_nOrder = nOrder; }
	// �X�V�����̎擾
	int GetOrder() { return m_nOrder; }

private:
	// �R���|�[�l���g���X�g��Order�̏��Ԃɏ����Ő���
	void SortComponent();

	// �R���|�[�l���g�̔j��
	void DestroyComponent();

private:
	vector<Component*> m_pComponents;		// �R���|�[�l���g���X�g
	vector<Component*> m_pDeadComponents;	// �j�����ꂽ�R���|�[�l���g�̃��X�g
	bool m_bIsDead;							// ���S�t���O : true�̏ꍇ�AActor�͍폜�����
	bool m_bIsActive;						// �A�N�e�B�u�t���O : false�̏ꍇ�AActor�͍X�V����Ȃ�
	std::string m_sTag;						// �^�O
	int m_nOrder;							// �X�V����
};

template<class T>
inline T* Actor::AddComponent()
{
	// �R���|�[�l���g�̐���
	T* pCmp = NEW T();
	pCmp->SetOwner(this);
	// �R���|�[�l���g�̒ǉ�
	m_pComponents.push_back(pCmp);
	// �R���|�[�l���g�̏�����
	((Component*)pCmp)->Init();
	// �R���|�[�l���g�̐���
	SortComponent();

	return pCmp;
}

template<class T>
inline T* Actor::GetComponent()
{
	for (Component* pCmp : m_pComponents)
	{
		if (typeid(*pCmp) == typeid(T))
		{
			return (T*)pCmp;
		}
	}
	return nullptr;
}

template<class T>
inline T* Actor::GetComponent(const string& tag)
{
	for (Component* pCmp : m_pComponents)
	{
		if (typeid(*pCmp) == typeid(T))
		{
			if (pCmp->GetTag() == tag)
			{
				return (T*)pCmp;
			}
		}
	}
	return nullptr;
}

