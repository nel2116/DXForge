// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [SceneManager.h]
// ����� : �c���~�m��
// ����� : 2024/06/06 18:00
// �T�v   : SceneManager�N���X�̒�` : Scene�N���X�̊Ǘ��N���X
// �X�V�� 
// 2024/06/06 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Manager.h"
#include <Scene/Scene.h>
#include <Manager/ActorManager.h>

// ====== �N���X�̒�` ======
class SceneManager : public Manager<SceneManager>
{
public:	// �����o�֐�
	// �X�V
	void Update();
	// �`��
	void Draw();
	// ������
	void Init();
	// �I��
	void Uninit();


public:	// �A�N�Z�T�֐�
	/// <summary>
	/// ���݂̃V�[����ύX
	/// </summary>
	/// <param name="sceneName">���̃V�[���̖��O</param>
	void ChangeScene(std::string sceneName);

	/// <summary>
	/// �|�[�Y�t���O��ݒ�
	/// </summary>
	/// <param name="flag">�t���O</param>
	void SetPauseFlag(bool flag) { m_bPauseFlag = flag; }

	/// <summary>
	/// ���݂̃V�[�����擾
	/// </summary>
	/// <returns>���݂̃V�[��</returns>
	Scene* GetNowScene() { return m_pNowScene; }

	/// <summary>
	/// PxPhysics�C���X�^���X���擾
	/// </summary>
	/// <returns>PxPhysics�C���X�^���X</returns>
	physx::PxPhysics* GetPhysics() { return m_pPhysics; }

private:	// �����o�ϐ�
	std::unordered_map<std::string, Scene*> m_pSceneMap;	// Scene�}�b�v
	Scene* m_pNowScene;										// ���݂�Scene
	Scene* m_pNextScene;									// ����Scene
	bool m_bPauseFlag;										// �|�[�Y�t���O
	physx::PxDefaultAllocator m_Allocator;					// PhysX�̃A���P�[�^
	physx::PxDefaultErrorCallback m_ErrorCallback;			// PhysX�̃G���[�R�[���o�b�N
	physx::PxFoundation* m_pFoundation = nullptr;			// ��ʃ��x����SDK���C���X�^���X������̂ɕK�v
	physx::PxPhysics* m_pPhysics = nullptr;					// ���ۂɕ������Z���s��
	physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;	// �������Z�̎��s���Ǘ�����(�V�~�����[�V�������ǂ��������邩�̐ݒ�Ń}���`�X���b�h�̐ݒ���ł���)
	physx::PxPvd* m_pPvd = nullptr;							// PVD�ƒʐM����ۂɕK�v

private:	// �V���O���g���֘A
	friend class Manager<SceneManager>;
	SceneManager()
		: m_pNowScene(nullptr)
		, m_pNextScene(nullptr)
		, m_bPauseFlag(false)
	{
	}

	static SceneManager* createInstance()
	{
		return NEW SceneManager();
	}
};
// �C���X�^���X�̎擾
#define SCENE_MANAGER SceneManager::manager()