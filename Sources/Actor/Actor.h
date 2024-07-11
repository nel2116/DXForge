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
/// @brief �A�N�^�[�N���X
/// @details �Q�[���I�u�W�F�N�g�̊��N���X
class Actor
{
public:	// �p�u���b�N�֐�
	/// @brief �R���X�g���N�^
	Actor() : m_bDestroy(false), m_bActive(true), m_sTag("none"), m_nOrder(0), m_bLooping(false) {}

	/// @brief �f�X�g���N�^
	virtual ~Actor() { DestroyComponent(); }

	/// @brief �X�V�����̏������z�֐�

	virtual void Update() = 0;

	/// <summary>
	/// �`�揈���̏������z�֐�
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// ����������
	/// </summary>
	virtual void Init() {}

	/// <summary>
	/// �I������
	/// </summary>
	virtual void Uninit() {}

	/// <summary>
	/// �X�V�����̊��֐�
	/// </summary>
	/// <remarks>�R���|�[�l���g => �A�N�^�[�̏��ԂōX�V</remarks>
	/// <remarks>Manager����Ăяo�����</remarks>
	void BaseUpdate();

	/// <summary>
	/// �`�揈���̊��֐�
	/// </summary>
	/// <remarks>�R���|�[�l���g => �A�N�^�[�̏��Ԃŕ`��</remarks>
	/// <remarks>Manager����Ăяo�����</remarks>
	void BaseDraw();

	/// <summary>
	/// �����������̊��֐�
	/// </summary>
	void BaseInit();

	/// <summary>
	/// �I�������̊��֐�
	/// </summary>
	void BaseUninit();

public:	// �A�N�Z�T

	/// <summary>
	/// �R���|�[�l���g�̒ǉ�
	/// </summary>
	/// <param name="T">�R���|�[�l���g�̌^</param>
	/// <param name="tag">�R���|�[�l���g�̃^�O</param>
	/// <returns>�ǉ������R���|�[�l���g�̃|�C���^</returns>
	/// <remarks>�R���|�[�l���g�̐����ƒǉ����s��</remarks>
	template<class T>
	T* AddComponent(std::string tag = "none");

	/// <summary>
	/// �R���|�[�l���g�̎擾
	/// </summary>
	/// <param name="T">�R���|�[�l���g�̌^</param>
	/// <returns>�擾�����R���|�[�l���g�̃|�C���^</returns>
	/// <remarks>�R���|�[�l���g�̎擾���s��</remarks>
	template<class T>
	T* GetComponent();

	/// <summary>
	/// �R���|�[�l���g�̎擾
	/// </summary>
	/// <param name="T">�R���|�[�l���g�̌^</param>
	/// <param name="tag">�R���|�[�l���g�̃^�O</param>
	/// <returns>�擾�����R���|�[�l���g�̃|�C���^</returns>
	/// <remarks>�^�O�Ŏw�肵�ăR���|�[�l���g�̎擾���s��</remarks>
	template<class T>
	T* GetComponent(const string& tag);

	/// <summary>
	/// ���g�̔j���t���O�𗧂Ă�
	/// </summary>
	/// <remarks>���g��j������</remarks>
	/// <remarks>�j�����ꂽ�A�N�^�[�͍폜�����</remarks>
	/// <remarks>�j�����ꂽ�A�N�^�[�̓R���|�[�l���g���j�������</remarks>
	void Destroy() { m_bDestroy = true; }

	/// <summary>
	///�j���S�t���O�̎擾
	/// </summary>
	/// <returns>�j���t���O</returns>
	/// <remarks>�j���t���O�̎擾���s��</remarks>
	bool IsDestroy() { return m_bDestroy; }

	/// <summary>
	/// �A�N�e�B�u�t���O�̐ݒ�
	/// </summary>
	/// <param name="bIsActive">�A�N�e�B�u�t���O</param>
	/// <remarks>�A�N�e�B�u�t���O�̐ݒ���s��</remarks>
	/// <remarks>�A�N�e�B�u�t���O��false�̏ꍇ�A�X�V����Ȃ�</remarks>
	void SetActive(bool bIsActive) { m_bActive = bIsActive; }

	/// <summary>
	/// �A�N�e�B�u�t���O�̎擾
	/// </summary>
	/// <returns>�A�N�e�B�u�t���O</returns>
	bool IsActive() { return m_bActive; }

	/// <summary>
	/// �^�O�̐ݒ�
	/// </summary>
	/// <param name="tag">�^�O</param>
	/// <remarks>�^�O�̐ݒ���s��</remarks>
	/// <remarks>�^�O��Actor�̎擾�Ɏg�p�����</remarks>
	void SetTag(const string& tag) { m_sTag = tag; }

	/// <summary>
	/// �^�O�̎擾
	/// </summary>
	/// <returns>�^�O</returns>
	/// <remarks>�^�O�̎擾���s��</remarks>
	/// <remarks>�^�O��Actor�̎擾�Ɏg�p�����</remarks>
	string GetTag() { return m_sTag; }

	/// <summary>
	/// �X�V�����̐ݒ�
	/// </summary>
	/// <param name="nOrder">�X�V����</param>
	/// <remarks>�X�V�����̐ݒ���s��</remarks>
	/// <remarks>Order�̏����ōX�V�����</remarks>
	void SetOrder(int nOrder) { m_nOrder = nOrder; }

	/// <summary>
	/// �X�V�����̎擾
	/// </summary>
	/// <returns>�X�V����</returns>
	/// <remarks>Order�̏����ōX�V�����</remarks>
	int GetOrder() { return m_nOrder; }

private:
	/// <summary>
	/// �R���|�[�l���g�̐���
	/// </summary>
	/// <remarks>�R���|�[�l���g�̐�����s��</remarks>
	/// <remarks>Order�̏����Ő��񂳂��</remarks>
	void SortComponent();

	/// <summary>
	/// �R���|�[�l���g�̔j��
	/// </summary>
	/// <remarks>�R���|�[�l���g�̔j�����s��</remarks>
	/// <remarks>�j�����ꂽ�R���|�[�l���g�̓��X�g����폜�����</remarks>
	void DestroyComponent();

private:
	/// <summary>
	/// �R���|�[�l���g���X�g
	/// </summary>
	/// <remarks>Actor�����R���|�[�l���g�̃��X�g</remarks>
	vector<Component*> m_pComponents;

	/// <summary>
	/// �ꎞ�I�ȃR���|�[�l���g���X�g
	/// </summary>
	/// <remarks>�R���|�[�l���g�̒ǉ����ɃC�e���[�^�����삳��Ȃ��悤�Ɉꎞ�I��Component��ێ�����z��</remarks>
	vector<Component*> m_pTempComponents;

	/// <summary>
	/// �j�����ꂽ�R���|�[�l���g���X�g
	/// </summary>
	/// <remarks>�j�����ꂽ�R���|�[�l���g�̃��X�g</remarks>
	/// <remarks>�j�����ꂽ�R���|�[�l���g�̓��X�g����폜�����</remarks>
	vector<Component*> m_pDeadComponents;

	/// <summary>
	/// �j���t���O
	/// </summary>
	/// <remarks>�j���t���O��true�̏ꍇ�AActor�͍폜�����</remarks>
	/// <remarks>�j�����ꂽActor�̓R���|�[�l���g���j�������</remarks>
	bool m_bDestroy;

	/// <summary>
	/// �A�N�e�B�u�t���O
	/// </summary>
	/// <remarks>�A�N�e�B�u�t���O��false�̏ꍇ�AActor�͍X�V����Ȃ�</remarks>
	/// <remarks>�A�N�e�B�u�t���O��false�̏ꍇ�A�R���|�[�l���g���X�V����Ȃ�</remarks>
	bool m_bActive;

	/// <summary>
	/// �^�O
	/// </summary>
	/// <remarks>Actor�̎擾�Ɏg�p�����</remarks>
	std::string m_sTag;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <remarks>Order�̏����ōX�V�����</remarks>
	int m_nOrder;

	/// <summary>
	/// ���[�v�t���O
	/// </summary>
	bool m_bLooping;
};

template<class T>
inline T* Actor::AddComponent(std::string tag)
{
	// �R���|�[�l���g�̐���
	T* pCmp = NEW T(tag);

	// �I�[�i�[�̐ݒ�
	pCmp->SetOwner(this);

	// �C�e���[�^�����삳��Ȃ��悤�A���[�v���͈ꎞ�I�ȃ��X�g�ɒǉ�����
	if (!m_bLooping)
	{
		// �R���|�[�l���g�̒ǉ�
		m_pComponents.push_back(pCmp);
		// �R���|�[�l���g�̐���
		SortComponent();
	}
	else
	{
		// �ꎞ�I�ȃ��X�g�ɒǉ�
		m_pTempComponents.push_back(pCmp);
	}

	// �R���|�[�l���g�̏�����
	((Component*)pCmp)->Init();

	return pCmp;
}

template<class T>
inline T* Actor::GetComponent()
{
	// �R���|�[�l���g�̎擾
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		if (typeid(*(*cmp)) == typeid(T))
		{
			return (T*)(*cmp);
		}
	}

	// ����Έꎞ�I�ȃ��X�g������T��
	if (m_pTempComponents.size() != 0)
	{
		auto temp = m_pTempComponents;
		for (auto cmp = temp.begin(), end = temp.end(); cmp != end; ++cmp)
		{
			if (typeid(*(*cmp)) == typeid(T))
			{
				return (T*)(*cmp);
			}
		}
	}

	return nullptr;
}

template<class T>
inline T* Actor::GetComponent(const string& tag)
{
	// �R���|�[�l���g�̎擾
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		if (typeid(*(*cmp)) == typeid(T) && (*cmp)->GetTag() == tag)
		{
			return (T*)(*cmp);
		}
	}
	// ����Έꎞ�I�ȃ��X�g������T��
	if (m_pTempComponents.size() != 0)
	{
		auto temp = m_pTempComponents;
		for (auto cmp = temp.begin(), end = temp.end(); cmp != end; ++cmp)
		{
			if (typeid(*(*cmp)) == typeid(T) && (*cmp)->GetTag() == tag)
			{
				return (T*)(*cmp);
			}
		}
	}

	return nullptr;
}

