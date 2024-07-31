// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Scene.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/26 12:00
// 概要   : Sceneクラスの定義	:	シーンの基底クラス
// 更新日
// 2024/06/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/MyMath.h>
#include <Graphics/Camera/Camera.h>

// ====== クラスの定義 ======
class Scene
{
public:	// メンバ関数
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() = 0;
	// 初期化
	virtual void Init() = 0;
	// 終了
	virtual void Uninit() = 0;

	// シュミレーション
	void Simulate();
	void InitPhysX(physx::PxPhysics* pPhysics, physx::PxDefaultCpuDispatcher* pDispatcher);
	void UninitPhysX();

	/// <summary>
	/// PhysXのシーンを取得
	/// </summary>
	/// <returns>PhysXのシーン</returns>
	physx::PxScene* GetPhysXScene() { return m_pScene; }

protected:	// メンバ変数
	RootSignature* m_pSceneRootSig;						// ルートシグニチャ
	PipelineState* m_pScenePSO;							// パイプラインステート
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];		// ライトバッファ
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// 変換用バッファ
	Camera* m_pCamera;									// カメラ
	Projector* m_pProjector;							// プロジェクター
	physx::PxScene* m_pScene = nullptr;				// 物理演算のシーン(シミュレーションする空間の単位でActorの追加などもここで行う)
};