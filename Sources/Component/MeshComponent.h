// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [MeshComponent.h]
// ����� : �c���~�m��
// �쐬�� : 2024/07/02
// �T�v   : ���b�V���R���|�[�l���g�̒�`
// �X�V��
// 2024/07/02 : �c���~�m�� : �V�K�쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Component.h"

// ====== �N���X�̒�` ======
class MeshComponent : public Component
{
public:	// �p�u���b�N�֐�
	// �R���X�g���N�^
	MeshComponent(std::string tag = "") : Component(tag) {}
	// �f�X�g���N�^
	~MeshComponent();

	// ������
	void Init() override;

	// �X�V
	void Update() override;

	// �`��
	void Draw() override;

	// �I������
	void Uninit() override;

private:
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];		// ���C�g�o�b�t�@
	ConstantBuffer m_CameraCB[FRAME_BUFFER_COUNT];		// �J�����o�b�t�@
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// �ϊ��p�o�b�t�@
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// ���b�V���p�o�b�t�@
	std::vector<Mesh*> m_pMesh;							// ���b�V��
	Material m_Material;								// �}�e���A��

};




