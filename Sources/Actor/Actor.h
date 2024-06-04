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
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Init() {}
	virtual void Uninit() {}

	void BaseUpdate();
	void BaseDraw();
	void BaseInit();
	void BaseUninit();



	// Actor�ɃI�u�W�F�N�g��ǉ�
	template<class T>
	T* AddComponent();


private:
	vector<Component*> m_pComponents;	// �R���|�[�l���g���X�g
	vector<Component*> m_pDeadComponents;	// �j�����ꂽ�R���|�[�l���g�̃��X�g
};

template<class T>
inline T* Actor::AddComponent()
{
	// �R���|�[�l���g�𐶐�
	T* cmp = NEW T();
	// �R���|�[�l���g�����X�g�ɒǉ�
	m_pComponents.emplace_back(cmp);
	cmp->SetParent(this);
	cmp->Init();
	return cmp;
}
