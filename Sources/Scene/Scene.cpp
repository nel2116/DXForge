#include "stdafx.h"
#include "Scene.h"

void Scene::Simulate()
{
	// �V�~�����[�V�������x���w�肷��
	m_pScene->simulate(TIMER_MANAGER.GetDeltaTime());
	// PhysX�̏������I���܂ő҂�
	m_pScene->fetchResults(true);
}

void Scene::InitPhysX(physx::PxPhysics* pPhysics, physx::PxDefaultCpuDispatcher* pDispatcher)
{
	// �V�[���̐ݒ�
	physx::PxSceneDesc scene_desc(pPhysics->getTolerancesScale());
	// �V�[���̏d�͂�ݒ�
	scene_desc.gravity = physx::PxVec3(0, -9, 0);
	// �V�[���̃t�B���^�[�V�F�[�_��ݒ�
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	// �V�[����CPU�f�B�X�p�b�`����ݒ�
	scene_desc.cpuDispatcher = pDispatcher;

	// ��Ԃ̃C���X�^���X��
	m_pScene = pPhysics->createScene(scene_desc);
	if (!m_pScene)
	{
		ELOG("Error : PhysX Scene Create Failed.");
	}
}

void Scene::UninitPhysX()
{
	if (m_pScene)
	{
		m_pScene->release();
		m_pScene = nullptr;
	}
}
