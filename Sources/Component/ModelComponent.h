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
#include <System/MyMath.h>

struct alignas(256) CbMesh
{
	DirectX::SimpleMath::Matrix   World;      // ���[���h�s��
};

struct alignas(256) CbMaterial
{
	DirectX::SimpleMath::Vector3 BaseColor;  // ��{�F
	float   Alpha;      // ���ߓx
	float   Roughness;  // �ʂ̑e��(�͈͂�0~1)
	float   Metallic;   // �����x(�͈͂�[0~1])
};

// ====== �N���X�̒�` ======
class ModelComponent : public Component
{
public:	// �p�u���b�N�֐�
	// �R���X�g���N�^
	ModelComponent(std::string tag = "") : Component(tag) {}
	// �f�X�g���N�^
	~ModelComponent();

	// ������
	void Init() override;

	// �X�V
	void Update() override;

	// �`��
	void Draw() override;

	// �I������
	void Uninit() override;

	// ���f���̓ǂݍ���
	void LoadModel(std::string fileName);

private:
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// ���b�V���p�o�b�t�@
	std::vector<Mesh*> m_pMesh;							// ���b�V��
	Material m_Material;								// �}�e���A��
};




