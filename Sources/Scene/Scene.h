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
	physx::PxScene* m_pScene = nullptr;				// �������Z�̃V�[��(�V�~�����[�V���������Ԃ̒P�ʂ�Actor�̒ǉ��Ȃǂ������ōs��)
};