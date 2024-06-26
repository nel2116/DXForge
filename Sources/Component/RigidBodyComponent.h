// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RigidBodyComponent.h]
// ����� : �c���~�m��
// ����� : 2024/06/26 17:00
// �T�v   : RigidBodyComponent�N���X�̒�`	:	���̃R���|�[�l���g�N���X
// �X�V��
// 2024/06/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Component.h"

using namespace DirectX::SimpleMath;

// ====== �N���X�̒�` ======
class RigidBodyComponent : public Component
{
public:	// �񋓑́E�\����
	// ���̂̎��
	enum ActorType
	{
		Static,
		Dynamic,
		Kinematic,
	};

	// �`��̎��
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

	// ������
	virtual void Init() override;
	// �I��
	virtual void Uninit() override;
	// �X�V
	virtual void Update() override;
	// �`��
	virtual void Draw() override;

public:	// �A�N�Z�T�֐�

	/// <summary>
	/// ���x�̐ݒ�(���������^��)
	/// </summary>
	/// <param name="velocity">���x</param>
	/// <param name="mode">�͂̎��</param>
	/// <returns></returns>
	void SetVelocity(Vector3 velocity, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	/// <summary>
	/// ���x�̐ݒ�(�����x)
	/// </summary>
	/// <param name="velocity">���x</param>
	/// <param name="mode">�͂̎��</param>
	void SetVelocityAdd(Vector3 velocity, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	/// <summary>
	/// �͂�������
	/// </summary>
	/// <param name="force">��</param>
	/// <param name="mode">�͂̎��</param>
	void AddForce(Vector3 force, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	/// <summary>
	/// ���̂̐ݒ�
	/// </summary>
	/// <param name="actorType"></param>
	/// <param name="shapeType"></param>
	void SetRigidBody(ActorType actorType = ActorType::Static, ShapeType shapeType = ShapeType::Box);

	/// <summary>
	/// �g�����X�t�H�[���̐ݒ�
	/// </summary>
	/// <param name="transform"></param>
	void SetTransform(Transform transform) { m_tTransform = transform; }

	/// <summary>
	/// �ʒu�̐ݒ�
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(Vector3 pos) { m_tTransform.position = pos; }

	/// <summary>
	/// ��]�̐ݒ�
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(Quaternion rot) { m_tTransform.rotation = rot; }

	/// <summary>
	/// �X�P�[���̐ݒ�
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(Vector3 scale) { m_tTransform.scale = scale; }

	/// <summary>
	/// �g�����X�t�H�[���̎擾
	/// </summary>
	/// <returns></returns>
	Transform GetTransform() { return m_tTransform; }

	/// <summary>
	/// �ʒu�̎擾
	/// </summary>
	/// <returns>�ʒu</returns>
	Vector3 GetPosition() { return m_tTransform.position; }

	/// <summary>
	/// ��]�̎擾
	/// </summary>
	/// <returns>��]</returns>
	Quaternion GetRotation() { return m_tTransform.rotation; }

	/// <summary>
	/// �X�P�[���̎擾
	/// </summary>
	/// <returns>�X�P�[��</returns>
	Vector3 GetScale() { return m_tTransform.scale; }

private:
	physx::PxActor* m_pActor;	// ����
	physx::PxShape* m_pShape;	// �`��
	physx::PxMaterial* m_pMaterial;	// �ގ�
	ActorType m_eActorType;		// ���̂̎��
	ShapeType m_eShapeType;		// �`��̎��
	Transform m_tTransform;		// �g�����X�t�H�[��
};
