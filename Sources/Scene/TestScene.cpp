#include "stdafx.h"
#include "TestScene.h"
#include <System/Input.h>
#include <Actor/TestActor.h>
#include <Component/RigidBodyComponent.h>

void TestScene::Update()
{
	auto body = ACTOR_MANAGER.GetActor<TestActor>()->GetComponent<RigidBodyComponent>();

	if (IsKeyPress('S'))
	{
		body->SetVelocity(Vector3(0.0f, 0.0f, 1.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('W'))
	{
		body->SetVelocity(Vector3(0.0f, 0.0f, -1.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('A'))
	{
		body->SetVelocity(Vector3(-1.0f, 0.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('D'))
	{
		body->SetVelocity(Vector3(1.0f, 0.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('Q'))
	{
		body->SetVelocity(Vector3(0.0f, 1.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}
	if (IsKeyPress('E'))
	{
		body->SetVelocity(Vector3(0.0f, -1.0f, 0.0f), physx::PxForceMode::eIMPULSE);
	}

}

void TestScene::Draw()
{
}

void TestScene::Init()
{
	auto pPhysics = SCENE_MANAGER.GetPhysics();

	// •½–Ê‚ð’Ç‰Á
	m_pScene->addActor(*physx::PxCreatePlane(
		*pPhysics, physx::PxPlane(0, 1, 0, 0),
		*pPhysics->createMaterial(0.5f, 0.5f, 0.5f))
	);

	TestActor* pActor = ACTOR_MANAGER.AddActor<TestActor>();
	auto body = pActor->AddComponent<RigidBodyComponent>();
	body->SetRigidBody(RigidBodyComponent::ActorType::Dynamic, RigidBodyComponent::ShapeType::Box);
}

void TestScene::Uninit()
{
}
