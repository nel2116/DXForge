// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ����� : �c���~�m��
// ����� : 2024/06/04 16:20
// �T�v   : Component�N���X�̃w�b�_�t�@�C��
// �X�V�� 
// 2024/06/04 16:20 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <string>

// ====== �N���X�̒�` ======
class Actor;
class Component
{
public:	// �����o�֐�
	Component() : m_pOwner(nullptr), m_bIsActive(true), m_bIsDead(false), m_nOrder(0), m_sTag("none") {}
	virtual ~Component() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Init() = 0;
	virtual void Uninit() = 0;

	void Destroy() { m_bIsDead = true; }

public:	// �A�N�Z�T
	void SetOwner(Actor* parent) { m_pOwner = parent; }
	Actor* GetOwner() { return m_pOwner; }
	void SetOrder(int nOrder) { m_nOrder = nOrder; }
	int GetOrder() { return m_nOrder; }
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	bool IsActive() { return m_bIsActive; }
	bool IsDead() { return m_bIsDead; }
	void SetTag(const std::string& tag) { m_sTag = tag; }
	std::string GetTag() { return m_sTag; }

protected:
	Actor* m_pOwner;	// �e�I�u�W�F�N�g

private:
	bool m_bIsActive;	// �A�N�e�B�u�t���O : false�̏ꍇ�A�R���|�[�l���g�͍X�V����Ȃ�
	bool m_bIsDead;		// ���S�t���O : true�̏ꍇ�A�R���|�[�l���g�͍폜�����
	int m_nOrder;		// �X�V����
	std::string m_sTag;	// �^�O
};