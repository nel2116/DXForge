// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Component.h]
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
	Component(std::string tag) : m_pOwner(nullptr), m_bIsActive(true), m_bIsDead(false), m_nOrder(0), m_sTag(tag) {}
	virtual ~Component() = default;

	/// @brief �X�V�����̏������z�֐�
	/// @remarks ������p�����čX�V�������L�q����
	virtual void Update() = 0;
	/// @brief �`�揈���̏������z�֐�
	/// @remarks ������p�����ĕ`�揈�����L�q����
	virtual void Draw() = 0;
	/// @brief �����������̏������z�֐�
	/// @remarks ������p�����ď������������L�q����
	virtual void Init() = 0;
	/// @brief �I�������̏������z�֐�
	/// @remarks ������p�����ďI���������L�q����
	virtual void Uninit() = 0;

	/// @brief �R���|�[�l���g�̔j��
	void Destroy() { m_bIsDead = true; }

public:	// �A�N�Z�T
	/// @brief �e�I�u�W�F�N�g�̐ݒ�
	/// @param parent �e�I�u�W�F�N�g
	void SetOwner(Actor* parent) { m_pOwner = parent; }
	/// @brief �e�I�u�W�F�N�g�̎擾
	/// @return Actor* �e�I�u�W�F�N�g
	Actor* GetOwner() { return m_pOwner; }
	/// @brief �X�V�����̐ݒ�
	/// @param nOrder �X�V����
	void SetOrder(int nOrder) { m_nOrder = nOrder; }
	/// @brief �X�V�����̎擾
	/// @return int �X�V����
	int GetOrder() { return m_nOrder; }
	/// @brief �A�N�e�B�u�t���O�̐ݒ�
	/// @param bIsActive �A�N�e�B�u�t���O
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	/// @brief �A�N�e�B�u�t���O�̎擾
	/// @return bool �A�N�e�B�u�t���O
	bool IsActive() { return m_bIsActive; }
	/// @brief ���S�t���O�̎擾
	/// @return bool ���S�t���O
	bool IsDestroy() { return m_bIsDead; }
	/// @brief �^�O�̐ݒ�
	/// @param tag �^�O
	void SetTag(const std::string& tag) { m_sTag = tag; }
	/// @brief �^�O�̎擾
	/// @return std::string �^�O
	std::string GetTag() { return m_sTag; }

protected:
	Actor* m_pOwner;	// �e�I�u�W�F�N�g

private:
	bool m_bIsActive;	// �A�N�e�B�u�t���O : false�̏ꍇ�A�R���|�[�l���g�͍X�V����Ȃ�
	bool m_bIsDead;		// ���S�t���O : true�̏ꍇ�A�R���|�[�l���g�͍폜�����
	int m_nOrder;		// �X�V����
	std::string m_sTag;	// �^�O
};