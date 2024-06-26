#include "stdafx.h"
#include "SceneManager.h"
#include <Scene/TestScene.h>

void SceneManager::Update()
{
	// ���̃V�[�����ݒ肳��Ă���ꍇ�̓V�[���̐؂�ւ�
	if (m_pNextScene)
	{
		// ���݂̃V�[���̏I������
		if (m_pNowScene) { m_pNowScene->Uninit(); m_pNowScene->UninitPhysX(); }
		// ���̃V�[�������݂̃V�[���ɐݒ�
		m_pNowScene = m_pNextScene;
		// �V�[���̏�����
		m_pNowScene->Init();
		// ���̃V�[����nullptr�ɂ���
		m_pNextScene = nullptr;
	}
	else
	{
		// nowScene��nullptr����Ȃ��ꍇ�͍X�V
		if (m_pNowScene)
		{
			// �|�[�Y�t���O�������Ă���ꍇ�͍X�V���s��Ȃ�
			if (!m_bPauseFlag)
			{
				// �V�~�����[�V����
				m_pNowScene->Simulate();
				// �X�V
				m_pNowScene->Update();
			}
		}
	}

	// ActorManager�̍X�V
	auto& actors = ACTOR_MANAGER;
	actors.Update();
}

void SceneManager::Draw()
{
	// nowScene��nullptr����Ȃ��ꍇ�͕`��
	if (m_pNowScene) m_pNowScene->Draw();
	// ActorManager�̕`��
	auto& actors = ACTOR_MANAGER;
	actors.Draw();
}

void SceneManager::Init()
{
	// PhysX�̏�����
	// Foundation�̃C���X�^���X��
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	if (!m_pFoundation)
	{
		ELOG("Error : PhysX Foundation Create Failed.");
	}

	// PVD�Ɛڑ�����ݒ�
	m_pPvd = physx::PxCreatePvd(*m_pFoundation);
	if (m_pPvd)
	{
		// PVD���̃f�t�H���g�|�[�g��5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		m_pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	}
	else
	{
		ELOG("Error : PhysX PVD Create Failed.");
	}

	// Physics�̃C���X�^���X��
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd);
	if (!m_pPhysics)
	{
		ELOG("Error : PhysX Physics Create Failed.");
	}
	// �g���@�\�p
	if (!PxInitExtensions(*m_pPhysics, m_pPvd))
	{
		ELOG("Error : PhysX Extensions Init Failed.");
	}
	// �����Ɏg���X���b�h���w�肷��
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
	std::string sceneName = "Test";

	// �V�[���̏�����
	m_pSceneMap[sceneName] = NEW TestScene();
	// m_pSceneMap["Game"];
	// m_pSceneMap["Result"];

	// �ŏ��̃V�[����ݒ�
	ChangeScene(sceneName);

	// ActorManager�̏�����
	auto& actors = ACTOR_MANAGER;
	actors.Init();

}

void SceneManager::Uninit()
{
	// ActorManager�̏I������
	auto& actors = ACTOR_MANAGER;
	actors.Uninit();
	// �V�[���̏I������
	if (m_pNowScene) m_pNowScene->Uninit();
	// �V�[���̉��
	for (auto& scene : m_pSceneMap)
	{
		SAFE_DELETE(scene.second);
	}
	m_pSceneMap.clear();
	// PhysX�̏I������
	PxCloseExtensions();
	if (m_pDispatcher)
	{
		m_pDispatcher->release();
		m_pDispatcher = nullptr;
	}
	if (m_pPhysics)
	{
		m_pPhysics->release();
		m_pPhysics = nullptr;
	}
	if (m_pPvd)
	{
		m_pPvd->disconnect();
		physx::PxPvdTransport* transport = m_pPvd->getTransport();
		m_pPvd->release();
		transport->release();
	}
	if (m_pFoundation)
	{
		m_pFoundation->release();
		m_pFoundation = nullptr;
	}
}

void SceneManager::ChangeScene(std::string sceneName)
{
	// ���̃V�[�����擾
	auto nextScene = m_pSceneMap[sceneName];
	// ���̃V�[�������݂��Ȃ��ꍇ�̓G���[
	if (!nextScene)
	{
		ELOG("Error : Scene Not Found. name = %s", sceneName.c_str());
		return;
	}
	// ���̃V�[����ݒ�
	m_pNextScene = nextScene;

	// PhysX�̏�����
	m_pNextScene->InitPhysX(m_pPhysics, m_pDispatcher);

	auto scene = m_pNextScene->GetPhysXScene();
	// PVD�̕\���ݒ�
	physx::PxPvdSceneClient* pvd_client;
	if (pvd_client = scene->getScenePvdClient(); pvd_client)
	{
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}
