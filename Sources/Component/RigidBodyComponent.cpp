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
	// ���̂��V�[������폜
	if (m_pActor) SCENE_MANAGER.GetNowScene()->GetPhysXScene()->removeActor(*m_pActor);
	// ���̂�j��
	if (m_pActor) m_pActor->release();
	// �`���j��
	if (m_pShape) m_pShape->release();
}

void RigidBodyComponent::Update()
{
	// ���̂̍X�V
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
	// ���x��ݒ�
	if (m_eActorType != ActorType::Static)
	{
		m_pActor->is<physx::PxRigidDynamic>()->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z), mode);
	}
}

void RigidBodyComponent::SetVelocityAdd(Vector3 velocity, physx::PxForceMode::Enum mode)
{
	// ���x��ݒ�
	if (m_eActorType != ActorType::Static)
	{
		m_pActor->is<physx::PxRigidDynamic>()->setAngularVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z), mode);
	}
}

void RigidBodyComponent::AddForce(Vector3 force, physx::PxForceMode::Enum mode)
{
	// �͂�������
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
		// �`��(Box)���쐬
		m_pShape = pPhysics->createShape(
			// Box�̑傫��
			physx::PxBoxGeometry(1.f, 1.f, 1.f),
			// ���C�W���Ɣ����W���̐ݒ�
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f)
		);

		break;
	case RigidBodyComponent::Sphere:
		// �`��(Sphere)���쐬
		m_pShape = pPhysics->createShape(
			// Sphere�̔��a
			physx::PxSphereGeometry(1.0f),
			// ���C�W���Ɣ����W���̐ݒ�
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	case RigidBodyComponent::Capsule:
		// �`��(Capsule)���쐬
		m_pShape = pPhysics->createShape(
			// Capsule�̔��a�ƍ���
			physx::PxCapsuleGeometry(1.0f, 1.0f),
			// ���C�W���Ɣ����W���̐ݒ�
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	case RigidBodyComponent::ConvexMesh:
		// �`��(ConvexMesh)���쐬
		m_pShape = pPhysics->createShape(
			// ConvexMesh�̌`��
			physx::PxConvexMeshGeometry(),
			// ���C�W���Ɣ����W���̐ݒ�
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	case RigidBodyComponent::StaticMesh:
		// �`��(StaticMesh)���쐬
		m_pShape = pPhysics->createShape(
			// StaticMesh�̌`��
			physx::PxTriangleMeshGeometry(),
			// ���C�W���Ɣ����W���̐ݒ�
			*pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
		break;
	}

	switch (m_eActorType)
	{
	case RigidBodyComponent::Static:
		m_pActor = pPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
		// ���̂Ɍ`���R�Â�
		m_pActor->is<physx::PxRigidStatic>()->attachShape(*m_pShape);
		break;
	case RigidBodyComponent::Dynamic:
		m_pActor = pPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		// ���̂Ɍ`���R�Â�
		m_pActor->is<physx::PxRigidDynamic>()->attachShape(*m_pShape);
		break;
	case RigidBodyComponent::Kinematic:
		m_pActor = pPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		// ���̂Ɍ`���R�Â�
		m_pActor->is<physx::PxRigidDynamic>()->attachShape(*m_pShape);
		break;
	}

	// ���̂��V�[���ɒǉ�
	pScene->addActor(*m_pActor);

	// ���̂̏�����
	m_pShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
}

