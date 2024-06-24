// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Camera.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/24 13:05 : �쐬
// �T�v   : �J�����N���X�̒�`
// �X�V����
// 2024/06/24  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <cstdint>
#include <d3d12.h>
#include <System/SimpleMath.h>

// ====== �N���X��` ======
class Camera
{
public: // �񋓑́E�\���̒�`
	// �C�x���g�^�C�v
	enum EventType
	{
		EventRotate = 0x1 << 0,		// ��]
		EventDolly = 0x1 << 1,		// �Y�[��
		EventMove = 0x1 << 2,		// �ړ�
		EventPanTilt = 0x1 << 3,	// �p���E�`���g
		EventReset = 0x1 << 4,		// ���Z�b�g
	};

	// �C�x���g�\����
	struct Event
	{
		uint32_t    Type = 0;       // �C�x���g�^�C�v
		float       RotateH = 0.0f;	// ���������̉�]�p(rad)
		float       RotateV = 0.0f; // ���������̉�]�p(rad)
		float       Pan = 0.0f;     // ���E�̎�U��p(rad)
		float       Tilt = 0.0f;    // �㉺�̎�U��p(rad)
		float       Dolly = 0.0f;   // �Y�[����
		float       MoveX = 0.0f;   // X�������ւ̈ړ���(�r���[���)
		float       MoveY = 0.0f;   // Y�������ւ̈ړ���(�r���[���)
		float       MoveZ = 0.0f;   // Z�������ւ̈ړ���(�r���[���)
	};

public:	//	�p�u���b�N�֐�
	// �R���X�g���N�^
	Camera();
	// �f�X�g���N�^
	~Camera();

	/// <summary>
	/// �ʒu��ݒ�
	/// </summary>
	/// <param name="position">�ʒu</param>
	void SetPosition(const DirectX::SimpleMath::Vector3& position);

	/// <summary>
	/// �����_��ݒ�
	/// </summary>
	/// <param name="target">�����_</param>
	void SetTarget(const DirectX::SimpleMath::Vector3& target);

	/// <summary>
	/// �C�x���g�̍X�V����
	/// </summary>
	/// <param name="event">�C�x���g</param>
	void UpdateEvent(const Event& event);

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �ۑ�
	/// </summary>
	void Preserve();

	/// <summary>
	/// ���Z�b�g
	/// </summary>
	void Reset();

public: // �A�N�Z�T�֐�
	/// <summary>
	/// �J�����̐���������]�p(rad)���擾
	/// </summary>
	/// <returns>����������]�p(rad)</returns>
	const float& GetAngleV() const;

	/// <summary>
	/// �J�����̐���������]�p(rad)���擾
	/// </summary>
	/// <returns>����������]�p(rad)</returns>
	const float& GetAngleH() const;

	/// <summary>
	/// �J�������璍���_�܂ł̋������擾
	/// </summary>
	/// <returns>����</returns>
	const float& GetDistance() const;

	/// <summary>
	/// �J�����̈ʒu���擾
	/// </summary>
	/// <returns>�ʒu</returns>
	const DirectX::SimpleMath::Vector3& GetPosition() const;

	/// <summary>
	/// �J�����̒����_���擾
	/// </summary>
	/// <returns>�����_</returns>
	const DirectX::SimpleMath::Vector3& GetTarget() const;

	/// <summary>
	/// �J�����̏�����x�N�g�����擾
	/// </summary>
	/// <returns>�����</returns>
	const DirectX::SimpleMath::Vector3& GetUpward() const;

	/// <summary>
	/// �J�����̑O�������擾
	/// </summary>
	/// <returns>�O����</returns>
	const DirectX::SimpleMath::Vector3& GetForward() const;

	/// <summary>
	/// �r���[�s����擾
	/// </summary>
	/// <returns>�r���[�s��</returns>
	const DirectX::SimpleMath::Matrix& GetView() const;

private:	// �v���C�x�[�g�񋓑́E�\����
	// �t���O
	enum DirtyFlag
	{
		DirtyNone = 0x0,			// �Čv�Z�Ȃ�
		DirtyPosition = 0x1 << 0,	// �ʒu���Čv�Z
		DirtyTarget = 0x1 << 1,		// �����_���Čv�Z
		DirtyAngle = 0x1 << 2,		// ��]�p���Čv�Z
		DirtyMatrix = 0x1 << 3,		// �s����Čv�Z
	};

	// �p�����[�^�\����
	struct Param
	{
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Target;
		DirectX::SimpleMath::Vector3 Upward;
		DirectX::SimpleMath::Vector3 Forward;
		DirectX::XMFLOAT2 Angle;
		float Distance;
	};

private:	// �v���C�x�[�g�֐�
	// ��]
	void Rotate(float angleH, float angleV);
	// �p���E�`���g
	void Pantilt(float angleH, float angleV);
	// �ړ�
	void Move(float moveX, float moveY, float moveZ);
	// �Y�[��
	void Dolly(float value);
	// �ʒu�̌v�Z
	void ComputePosition();
	// �����_�̌v�Z
	void ComputeTarget();
	// ��]�p�̌v�Z
	void ComputeAngle();

private:	// �����o�ϐ�
	Param m_Current = {};	// ���݂̃p�����[�^
	Param m_Preserve = {};	// �ۑ��p�p�����[�^
	DirectX::SimpleMath::Matrix m_View = DirectX::SimpleMath::Matrix::Identity;	// �r���[�s��
	uint32_t m_DirtyFlag = 0;	// �t���O

};

class Projector
{
public:	// �񋓑̒�`
	enum Mode
	{
		Perspective,        //!< �������e.
		Orhographic,        //!< ���ˉe.
	};

public:	// �p�u���b�N�֐�
	// �R���X�g���N�^
	Projector();
	// �f�X�g���N�^
	~Projector();

	/// <summary>
	/// ���݂̎ˉe�p�����[�^��ۑ�
	/// </summary>
	void Preserve();
	/// <summary>
	/// �ˉe�p�����[�^�����Z�b�g
	/// </summary>
	void Reset();

	/// <summary>
	/// �������e�p�����[�^��ݒ�
	/// </summary>
	/// <param name="fov"></param>
	/// <param name="aspect"></param>
	/// <param name="nearClip"></param>
	/// <param name="farClip"></param>
	void SetPerspective(float fov, float aspect, float nearClip, float farClip);

	/// <summary>
	/// ���ˉe�p�����[�^��ݒ�
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <param name="top"></param>
	/// <param name="bottom"></param>
	/// <param name="nearClip"></param>
	/// <param name="farClip"></param>
	void SetOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip);

public:	// �A�N�Z�T�֐�
	/// <summary>
	/// �ˉe���[�h���擾
	/// </summary>
	/// <returns>�ˉe���[�h</returns>
	const Mode& GetMode() const;

	/// <summary>
	/// ������p���擾
	/// </summary>
	/// <returns>������p</returns>
	const float& GetFieldOfView() const;

	/// <summary>
	/// �A�X�y�N�g����擾
	/// </summary>
	/// <returns>�A�X�y�N�g��</returns>
	const float& GetAspect() const;

	/// <summary>
	/// �߃N���b�v���ʂ܂ł̋������擾
	/// </summary>
	/// <returns>�߃N���b�v���ʂ܂ł̋���</returns>
	const float& GetNearClip() const;

	/// <summary>
	/// ���N���b�v���ʂ܂ł̋������擾
	/// </summary>
	/// <returns>���N���b�v���ʂ܂ł̋���</returns>
	const float& GetFarClip() const;

	/// <summary>
	/// �ˉe�s����擾
	/// </summary>
	/// <returns>�ˉe�s��</returns>
	const DirectX::SimpleMath::Matrix& GetMatrix() const;

private:	// �v���C�x�[�g�񋓑́E�\����
	struct Param
	{
		Mode Mode;          // �ˉe���[�h
		float Aspect;       // �A�X�y�N�g��
		float FieldOfView;	// ������p
		float Left;         // ���[
		float Right;        // �E�[
		float Top;          // ��[
		float Bottom;       // ���[
		float NearClip;     // �߃N���b�v���ʂ܂ł̋���
		float FarClip;      // ���N���b�v���ʂ܂ł̋���
	};

private:	// �����o�ϐ�
	Param m_Current;					// ���݂̃p�����[�^
	Param m_Preserve;					// �ۑ��p�p�����[�^
	DirectX::SimpleMath::Matrix m_Proj; // �ˉe�s��
};