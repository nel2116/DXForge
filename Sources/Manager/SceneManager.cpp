#include "stdafx.h"
#include "SceneManager.h"
#include <Scene/TestScene.h>

void SceneManager::Update()
{
	// 次のシーンが設定されている場合はシーンの切り替え
	if (m_pNextScene)
	{
		// 現在のシーンの終了処理
		if (m_pNowScene) { m_pNowScene->Uninit(); m_pNowScene->UninitPhysX(); }
		// 次のシーンを現在のシーンに設定
		m_pNowScene = m_pNextScene;
		// シーンの初期化
		m_pNowScene->Init();
		// 次のシーンをnullptrにする
		m_pNextScene = nullptr;
	}
	else
	{
		// nowSceneがnullptrじゃない場合は更新
		if (m_pNowScene)
		{
			// ポーズフラグが立っている場合は更新を行わない
			if (!m_bPauseFlag)
			{
				// シミュレーション
				m_pNowScene->Simulate();
				// 更新
				m_pNowScene->Update();
			}
		}
	}

	// ActorManagerの更新
	auto& actors = ACTOR_MANAGER;
	actors.Update();
}

void SceneManager::Draw()
{
	// nowSceneがnullptrじゃない場合は描画
	if (m_pNowScene) m_pNowScene->Draw();
	// ActorManagerの描画
	auto& actors = ACTOR_MANAGER;
	actors.Draw();
}

void SceneManager::Init()
{
	// PhysXの初期化
	// Foundationのインスタンス化
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	if (!m_pFoundation)
	{
		ELOG("Error : PhysX Foundation Create Failed.");
	}

	// PVDと接続する設定
	m_pPvd = physx::PxCreatePvd(*m_pFoundation);
	if (m_pPvd)
	{
		// PVD側のデフォルトポートは5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		m_pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	}
	else
	{
		ELOG("Error : PhysX PVD Create Failed.");
	}

	// Physicsのインスタンス化
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd);
	if (!m_pPhysics)
	{
		ELOG("Error : PhysX Physics Create Failed.");
	}
	// 拡張機能用
	if (!PxInitExtensions(*m_pPhysics, m_pPvd))
	{
		ELOG("Error : PhysX Extensions Init Failed.");
	}
	// 処理に使うスレッドを指定する
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
	std::string sceneName = "Test";

	// シーンの初期化
	m_pSceneMap[sceneName] = NEW TestScene();
	// m_pSceneMap["Game"];
	// m_pSceneMap["Result"];

	// 最初のシーンを設定
	ChangeScene(sceneName);

	// ActorManagerの初期化
	auto& actors = ACTOR_MANAGER;
	actors.Init();

}

void SceneManager::Uninit()
{
	// ActorManagerの終了処理
	auto& actors = ACTOR_MANAGER;
	actors.Uninit();
	// シーンの終了処理
	if (m_pNowScene) m_pNowScene->Uninit();
	// シーンの解放
	for (auto& scene : m_pSceneMap)
	{
		SAFE_DELETE(scene.second);
	}
	m_pSceneMap.clear();
	// PhysXの終了処理
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
	// 次のシーンを取得
	auto nextScene = m_pSceneMap[sceneName];
	// 次のシーンが存在しない場合はエラー
	if (!nextScene)
	{
		ELOG("Error : Scene Not Found. name = %s", sceneName.c_str());
		return;
	}
	// 次のシーンを設定
	m_pNextScene = nextScene;

	// PhysXの初期化
	m_pNextScene->InitPhysX(m_pPhysics, m_pDispatcher);

	auto scene = m_pNextScene->GetPhysXScene();
	// PVDの表示設定
	physx::PxPvdSceneClient* pvd_client;
	if (pvd_client = scene->getScenePvdClient(); pvd_client)
	{
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}
