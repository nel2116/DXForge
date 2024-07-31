// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Scene.h]
// ����� : �c���~�m��
// ����� : 2024/06/26 12:00
// �T�v   : Scene�N���X�̒�`	:	�V�[���̊��N���X
// �X�V��
// 2024/06/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/MyMath.h>
#include <Graphics/Camera/Camera.h>

// ====== �N���X�̒�` ======
class Scene
{
public:	// �����o�֐�
	// �X�V
	virtual void Update() = 0;
	// �`��
	virtual void Draw() = 0;
	// ������
	virtual void Init() = 0;
	// �I��
	virtual void Uninit() = 0;

	// �V���~���[�V����
	void Simulate();
	void InitPhysX(physx::PxPhysics* pPhysics, physx::PxDefaultCpuDispatcher* pDispatcher);
	void UninitPhysX();

	/// <summary>
	/// PhysX�̃V�[�����擾
	/// </summary>
	/// <returns>PhysX�̃V�[��</returns>
	physx::PxScene* GetPhysXScene() { return m_pScene; }

protected:	// �����o�ϐ�
	RootSignature* m_pSceneRootSig;						// ���[�g�V�O�j�`��
	PipelineState* m_pScenePSO;							// �p�C�v���C���X�e�[�g
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];		// ���C�g�o�b�t�@
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// �ϊ��p�o�b�t�@
	Camera* m_pCamera;									// �J����
	Projector* m_pProjector;							// �v���W�F�N�^�[
	physx::PxScene* m_pScene = nullptr;				// �������Z�̃V�[��(�V�~�����[�V���������Ԃ̒P�ʂ�Actor�̒ǉ��Ȃǂ������ōs��)
};