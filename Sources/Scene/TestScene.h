// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [TestScene.h]
// ����� : �c���~�m��
// ����� : 2024/06/26 12:00
// �T�v   : TestScene�N���X�̒�`	:	�e�X�g�V�[���N���X
// �X�V��
// 2024/06/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Scene.h"

// ====== �N���X�̒�` ======
class TestScene : public Scene
{
public:	// �����o�֐�
	// �X�V
	void Update() override;
	// �`��
	void Draw() override;
	// ������
	void Init() override;
	// �I��
	void Uninit() override;

private:	// �����o�ϐ�

};

