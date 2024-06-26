#include "stdafx.h"
#include "RigidBodyComponent.h"

RigidBodyComponent::RigidBodyComponent(std::string tag)
	: Component(tag)
	, m_pActor(nullptr)
	, m_pShape(nullptr)
	, m_pMaterial(nullptr)
	, m_eActorType()
	, m_eShapeType()
{
}

void RigidBodyComponent::Init()
{
}

void RigidBodyComponent::Uninit()
{
	// „‘Ì‚ðƒV[ƒ“‚©‚çíœ
	if (m_pActor) SCENE_MANAGER.GetNowScene()->GetPhysXScene()->removeActor(*m_pActor);
	// „‘Ì‚ð”jŠü
	if (m_pActor) m_pActor->release();
	// Œ`ó‚ð”jŠü
	if (m_pShape) m_pShape->release();
}

void RigidBodyComponent::Update()
{
	// „‘Ì‚ÌXV
	if (m_pActor)
	{
		auto trans = m_pActor->is<physx::PxRigidActor>()->getGlobalPose();
		m_tTransform = ToTransform(trans);
	}
}

void RigidBodyComponent::Draw()
{
}

void RigidBodyComponent::SetVelocity(Vector3 velocity, physx::PxForceMode::Enum mode)
{
	// ‘¬“x‚ðÝ’è
	if (m_eActorType != ActorType::Static)
	{
		m_pActor->is<physx::PxRigidDynamic>()->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z), mode);
	}
}

void RigidBodyComponent::SetVelocityAdd(Vector3 velocity, physx::PxForceMode::Enum mode)
{
	// ‘¬“x‚ðÝ’è
	if (m_eActorType != ActorType::Static)
	{
		m_pActor->is<physx::PxRigidDynamic>()->setAngularVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z), mode);
	}
}

void RigidBodyComponent::AddForce(Vector3 force, physx::PxForceMode::Enum mode)
{
	// —Í‚ð‰Á‚¦‚é
	if (m_eActorType != ActorType::Static)
	{
		m_pActor->is<physx::PxRigidDynamic>()->addForce(physx::PxVec3(force.x, force.y, force.z), mode);
	}
}

void RigidBodyComponent::SetRigidBody(ActorType actorType, ShapeType shapeType)
{
	m_eActorType = actorType;
	m_eShapeType = shapeType;
	auto pPhysics = SCENE_MANAGER.GetPhysics();
	auto pScene = SCENE_MANAGER.GetNowScene()->GetPhysXScene();

	switch (m_eShapeType)
	{
	case RigidBodyComponent::Box:
		// Œ`ó(Box)‚ðì¬
		m_pShape = pPhysics->createShape(
			// Box‚Ì‘å‚«‚³
			physx::PxBoxGeometry(1.f, 1.f, 1.f),
			// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f)
		);

		break;
	case RigidBodyComponent::Sphere:
		// Œ`ó(Sphere)‚ðì¬
		m_pShape = pPhysics->createShape(
			// Sphere‚Ì”¼Œa
			physx::PxSphereGeometry(1.0f),
			// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	case RigidBodyComponent::Capsule:
		// Œ`ó(Capsule)‚ðì¬
		m_pShape = pPhysics->createShape(
			// Capsule‚Ì”¼Œa‚Æ‚‚³
			physx::PxCapsuleGeometry(1.0f, 1.0f),
			// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	case RigidBodyComponent::ConvexMesh:
		// Œ`ó(ConvexMesh)‚ðì¬
		m_pShape = pPhysics->createShape(
			// ConvexMesh‚ÌŒ`ó
			physx::PxConvexMeshGeometry(),
			// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	case RigidBodyComponent::StaticMesh:
		// Œ`ó(StaticMesh)‚ðì¬
		m_pShape = pPhysics->createShape(
			// StaticMesh‚ÌŒ`ó
			physx::PxTriangleMeshGeometry(),
			// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	}

	switch (m_eActorType)
	{
	case RigidBodyComponent::Static:
		m_pActor = pPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
		// „‘Ì‚ÉŒ`ó‚ð•R‚Ã‚¯
		m_pActor->is<physx::PxRigidStatic>()->attachShape(*m_pShape);
		break;
	case RigidBodyComponent::Dynamic:
		m_pActor = pPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		// „‘Ì‚ÉŒ`ó‚ð•R‚Ã‚¯
		m_pActor->is<physx::PxRigidDynamic>()->attachShape(*m_pShape);
		break;
	case RigidBodyComponent::Kinematic:
		m_pActor = pPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		// „‘Ì‚ÉŒ`ó‚ð•R‚Ã‚¯
		m_pActor->is<physx::PxRigidDynamic>()->attachShape(*m_pShape);
		break;
	}

	// „‘Ì‚ðƒV[ƒ“‚É’Ç‰Á
	pScene->addActor(*m_pActor);

	// „‘Ì‚Ì‰Šú‰»
	m_pShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
}

