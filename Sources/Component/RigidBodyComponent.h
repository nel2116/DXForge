// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RigidBodyComponent.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/26 17:00
// 概要   : RigidBodyComponentクラスの定義	:	剛体コンポーネントクラス
// 更新日
// 2024/06/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Component.h"

using namespace DirectX::SimpleMath;

// ====== クラスの定義 ======
class RigidBodyComponent : public Component
{
public:	// 列挙体・構造体
	// 剛体の種類
	enum ActorType
	{
		Static,
		Dynamic,
		Kinematic,
	};

	// 形状の種類
	enum ShapeType
	{
		Box,
		Sphere,
		Capsule,
		ConvexMesh,
		StaticMesh,
	};

public:
	RigidBodyComponent(std::string tag);

	// 初期化
	virtual void Init() override;
	// 終了
	virtual void Uninit() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;

public:	// アクセサ関数

	/// <summary>
	/// 速度の設定(等速直線運動)
	/// </summary>
	/// <param name="velocity">速度</param>
	/// <param name="mode">力の種類</param>
	/// <returns></returns>
	void SetVelocity(Vector3 velocity, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	/// <summary>
	/// 速度の設定(加速度)
	/// </summary>
	/// <param name="velocity">速度</param>
	/// <param name="mode">力の種類</param>
	void SetVelocityAdd(Vector3 velocity, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	/// <summary>
	/// 力を加える
	/// </summary>
	/// <param name="force">力</param>
	/// <param name="mode">力の種類</param>
	void AddForce(Vector3 force, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	/// <summary>
	/// 剛体の設定
	/// </summary>
	/// <param name="actorType"></param>
	/// <param name="shapeType"></param>
	void SetRigidBody(ActorType actorType = ActorType::Static, ShapeType shapeType = ShapeType::Box);

	/// <summary>
	/// トランスフォームの設定
	/// </summary>
	/// <param name="transform"></param>
	void SetTransform(Transform transform) { m_tTransform = transform; }

	/// <summary>
	/// 位置の設定
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(Vector3 pos) { m_tTransform.position = pos; }

	/// <summary>
	/// 回転の設定
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(Quaternion rot) { m_tTransform.rotation = rot; }

	/// <summary>
	/// スケールの設定
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(Vector3 scale) { m_tTransform.scale = scale; }

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	/// <returns></returns>
	Transform GetTransform() { return m_tTransform; }

	/// <summary>
	/// 位置の取得
	/// </summary>
	/// <returns>位置</returns>
	Vector3 GetPosition() { return m_tTransform.position; }

	/// <summary>
	/// 回転の取得
	/// </summary>
	/// <returns>回転</returns>
	Quaternion GetRotation() { return m_tTransform.rotation; }

	/// <summary>
	/// スケールの取得
	/// </summary>
	/// <returns>スケール</returns>
	Vector3 GetScale() { return m_tTransform.scale; }

private:
	physx::PxActor* m_pActor;	// 剛体
	physx::PxShape* m_pShape;	// 形状
	physx::PxMaterial* m_pMaterial;	// 材質
	ActorType m_eActorType;		// 剛体の種類
	ShapeType m_eShapeType;		// 形状の種類
	Transform m_tTransform;		// トランスフォーム
};
