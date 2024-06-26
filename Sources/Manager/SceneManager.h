// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [SceneManager.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/06 18:00
// 概要   : SceneManagerクラスの定義 : Sceneクラスの管理クラス
// 更新日 
// 2024/06/06 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Manager.h"
#include <Scene/Scene.h>
#include <Manager/ActorManager.h>

// ====== クラスの定義 ======
class SceneManager : public Manager<SceneManager>
{
public:	// メンバ関数
	// 更新
	void Update();
	// 描画
	void Draw();
	// 初期化
	void Init();
	// 終了
	void Uninit();


public:	// アクセサ関数
	/// <summary>
	/// 現在のシーンを変更
	/// </summary>
	/// <param name="sceneName">次のシーンの名前</param>
	void ChangeScene(std::string sceneName);

	/// <summary>
	/// ポーズフラグを設定
	/// </summary>
	/// <param name="flag">フラグ</param>
	void SetPauseFlag(bool flag) { m_bPauseFlag = flag; }

	/// <summary>
	/// 現在のシーンを取得
	/// </summary>
	/// <returns>現在のシーン</returns>
	Scene* GetNowScene() { return m_pNowScene; }

	/// <summary>
	/// PxPhysicsインスタンスを取得
	/// </summary>
	/// <returns>PxPhysicsインスタンス</returns>
	physx::PxPhysics* GetPhysics() { return m_pPhysics; }

private:	// メンバ変数
	std::unordered_map<std::string, Scene*> m_pSceneMap;	// Sceneマップ
	Scene* m_pNowScene;										// 現在のScene
	Scene* m_pNextScene;									// 次のScene
	bool m_bPauseFlag;										// ポーズフラグ
	physx::PxDefaultAllocator m_Allocator;					// PhysXのアロケータ
	physx::PxDefaultErrorCallback m_ErrorCallback;			// PhysXのエラーコールバック
	physx::PxFoundation* m_pFoundation = nullptr;			// 上位レベルのSDKをインスタンス化するのに必要
	physx::PxPhysics* m_pPhysics = nullptr;					// 実際に物理演算を行う
	physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;	// 物理演算の実行を管理する(シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる)
	physx::PxPvd* m_pPvd = nullptr;							// PVDと通信する際に必要

private:	// シングルトン関連
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
// インスタンスの取得
#define SCENE_MANAGER SceneManager::manager()