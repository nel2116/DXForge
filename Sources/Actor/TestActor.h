// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [TestActor.h]
// ����� : �c���~�m��
// ����� : 2024/06/26 12:00
// �T�v   : TestActor�N���X�̒�`	:	�e�X�g�A�N�^�[�N���X
// �X�V��
// 2024/06/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Actor.h"

// ====== �N���X�̒�` ======
class TestActor : public Actor
{
public:	// �����o�֐�
	// ������
	void Init() override;
	// �I��
	void Uninit() override;
	// �X�V
	void Update() override;
	// �`��
	void Draw() override;

private:	// �����o�ϐ�

};


