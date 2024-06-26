#include "stdafx.h"
#include "Scene.h"

void Scene::Simulate()
{
	// シミュレーション速度を指定する
	m_pScene->simulate(TIMER_MANAGER.GetDeltaTime());
	// PhysXの処理が終わるまで待つ
	m_pScene->fetchResults(true);
}

void Scene::InitPhysX(physx::PxPhysics* pPhysics, physx::PxDefaultCpuDispatcher* pDispatcher)
{
	// シーンの設定
	physx::PxSceneDesc scene_desc(pPhysics->getTolerancesScale());
	// シーンの重力を設定
	scene_desc.gravity = physx::PxVec3(0, -9, 0);
	// シーンのフィルターシェーダを設定
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	// シーンのCPUディスパッチャを設定
	scene_desc.cpuDispatcher = pDispatcher;

	// 空間のインスタンス化
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
