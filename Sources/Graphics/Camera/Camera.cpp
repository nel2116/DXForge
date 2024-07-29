// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Camera.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/24 13:05 : �쐬
// �T�v   : �J�����N���X�̎���
// �X�V����
// 2024/06/24  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Camera.h"

namespace
{
	float Cos(float rad)
	{
		if (abs(rad) < FLT_EPSILON)
		{
			return 1.0f;
		}

		return cosf(rad);
	}

	float Sin(float rad)
	{
		if (abs(rad) < FLT_EPSILON)
		{
			return 0.0f;
		}

		return sinf(rad);
	}

	// �p�x�����߂�
	float CalcAngle(float sine, float cosine)
	{
		auto result = asinf(sine);
		if (cosine < 0.0f)
		{
			result = DirectX::XM_PI - result;
		}

		return result;
	}

	// �w��x�N�g������p�x�Ƌ��������߂�
	void ToAngle(const DirectX::SimpleMath::Vector3& v, float* angleH, float* angleV, float* dist)
	{
		if (dist != nullptr)
		{
			*dist = v.Length();
		}

		DirectX::SimpleMath::Vector3 src(-v.x, 0.0f, -v.z);
		DirectX::SimpleMath::Vector3 dst = src;

		if (angleH != nullptr)
		{
			// �P�ʃx�N�g����
			if (fabs(src.x) > FLT_EPSILON || fabs(src.z) > FLT_EPSILON)
			{
				src.Normalize(dst);
			}

			*angleH = CalcAngle(dst.x, dst.z);
		}

		if (angleV != nullptr)
		{
			auto d = src.Length();
			src.x = d;
			src.y = -v.y;
			src.z = 0.0f;

			dst = src;

			// �P�ʃx�N�g����
			if (fabs(src.x) > FLT_EPSILON || fabs(src.y) > FLT_EPSILON)
			{
				src.Normalize(dst);
			}

			*angleV = CalcAngle(dst.y, dst.x);
		}
	}

	// �w��p�x���王���x�N�g���Ə�����x�N�g�������߂�
	void ToVector(float angleH, float angleV, DirectX::SimpleMath::Vector3* forward, DirectX::SimpleMath::Vector3* upward)
	{
		auto sx = Sin(angleH);
		auto cx = Cos(angleH);
		auto sy = Sin(angleV);
		auto cy = Cos(angleV);

		if (forward != nullptr)
		{
			forward->x = -cy * sx;
			forward->y = -sy;
			forward->z = -cy * cx;
		}

		if (upward != nullptr)
		{
			upward->x = -sy * sx;
			upward->y = cy;
			upward->z = -sy * cx;
		}
	}

} // namespace

// ====== �J�����N���X ======
// �R���X�g���N�^
Camera::Camera()
	: m_Current({ DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f),
				 DirectX::SimpleMath::Vector3::Zero,
				 DirectX::SimpleMath::Vector3::UnitY,
				 DirectX::SimpleMath::Vector3::UnitZ,
				 {0.0f, 0.0f},
				 1.0f }),
	m_Preserve(m_Current),
	m_DirtyFlag(DirtyPosition)
{
}


// �f�X�g���N�^
Camera::~Camera()
{
}

// �J�����ʒu��ݒ�
void Camera::SetPosition(const DirectX::SimpleMath::Vector3& value)
{
	m_Current.Position = value;
	// ComputeTarget();
	m_DirtyFlag = DirtyAngle;
	Update();
}

// �����_��ݒ�
void Camera::SetTarget(const DirectX::SimpleMath::Vector3& value)
{
	m_Current.Target = value;
	//	ComputePosition();
	m_DirtyFlag = DirtyAngle;
	Update();
}

// �J�����C�x���g����ɍX�V�������s��
void Camera::UpdateEvent(const Event& value)
{
	if (value.Type & EventRotate)
	{
		Rotate(value.RotateH, value.RotateV);
	}

	if (value.Type & EventPanTilt)
	{
		Pantilt(value.Pan, value.Tilt);
	}

	if (value.Type & EventDolly)
	{
		Dolly(value.Dolly);
	}

	if (value.Type & EventMove)
	{
		Move(value.MoveX, value.MoveY, value.MoveZ);
	}

	if (value.Type & EventReset)
	{
		Reset();
		return;
	}

	Update();
}

// �X�V����
// �r���[�s����Čv�Z����ۂɁA�t���O�̐ݒ�ƒl�̊m�F���s��
void Camera::Update()
{
	if (m_DirtyFlag != DirtyNone)
	{
		// �K�v�ɉ����āA�t���O���ƂɍČv�Z�����{
		if (m_DirtyFlag & DirtyPosition)
		{
			ComputePosition();
		}

		if (m_DirtyFlag & DirtyTarget)
		{
			ComputeTarget();
		}

		if (m_DirtyFlag & DirtyAngle)
		{
			ComputeAngle();
		}

		// �r���[�s����v�Z
		m_View = DirectX::SimpleMath::Matrix::CreateLookAt(
			m_Current.Position,
			m_Current.Target,
			m_Current.Upward);

		// �t���O���N���A
		m_DirtyFlag = DirtyNone;
	}
}

// ���݂̃J�����p�����[�^��ۑ�
void Camera::Preserve()
{
	m_Preserve = m_Current;
}

// �J�����p�����[�^�����Z�b�g
void Camera::Reset()
{
	m_Current = m_Preserve;
	m_DirtyFlag = DirtyMatrix;
	Update();
}

// �J�����̐���������]�p(rad)���擾
const float& Camera::GetAngleH() const
{
	return m_Current.Angle.x;
}

// �J�����̐���������]�p(rad)���擾
const float& Camera::GetAngleV() const
{
	return m_Current.Angle.y;
}

// �J�������璍���_�܂ł̋������擾
const float& Camera::GetDistance() const
{
	return m_Current.Distance;
}

// �J�����ʒu���擾
const DirectX::SimpleMath::Vector3& Camera::GetPosition() const
{
	return m_Current.Position;
}

// �����_���擾
const DirectX::SimpleMath::Vector3& Camera::GetTarget() const
{
	return m_Current.Target;
}

// �J�����̏�����x�N�g�����擾
const DirectX::SimpleMath::Vector3& Camera::GetUpward() const
{
	return m_Current.Upward;
}

// �J�����̑O�������擾
const DirectX::SimpleMath::Vector3& Camera::GetForward() const
{
	return m_Current.Forward;
}

// �r���[�s����擾
const DirectX::SimpleMath::Matrix& Camera::GetView() const
{
	return m_View;
}

// �����_�����Ƃ��ĉ�]�������s��
void Camera::Rotate(float angleH, float angleV)
{
	ComputeAngle();
	ComputeTarget();

	m_Current.Angle.x += angleH;
	m_Current.Angle.y += angleV;

	// �W���o�����b�N�΍�
	{
		if (m_Current.Angle.y > DirectX::XM_PIDIV2 - FLT_EPSILON)
		{
			m_Current.Angle.y = DirectX::XM_PIDIV2 - FLT_EPSILON;
		}

		if (m_Current.Angle.y < -DirectX::XM_PIDIV2 + FLT_EPSILON)
		{
			m_Current.Angle.y = -DirectX::XM_PIDIV2 + FLT_EPSILON;
		}
	}

	m_DirtyFlag |= DirtyPosition;
}

// ��U�菈�����s��
void Camera::Pantilt(float angleH, float angleV)
{
	ComputeAngle();
	ComputePosition();

	m_Current.Angle.x += angleH;
	m_Current.Angle.y += angleV;

	m_DirtyFlag |= DirtyTarget;
}

// �r���[��Ԃ̊��x�N�g�������Ɉړ�������
void Camera::Move(float moveX, float moveY, float moveZ)
{
	auto translate = m_View.Right() * moveX + m_View.Up() * moveY + m_View.Forward() * moveZ;

	m_Current.Position += translate;
	m_Current.Target += translate;

	m_DirtyFlag |= DirtyMatrix;
}

// �Y�[������
void Camera::Dolly(float value)
{
	ComputeAngle();
	ComputeTarget();

	m_Current.Distance += value;
	if (m_Current.Distance < 0.001f)
	{
		m_Current.Distance = 0.001f;
	}

	m_DirtyFlag |= DirtyPosition;
}

// �J�����ʒu���v�Z
void Camera::ComputePosition()
{
	ToVector(m_Current.Angle.x, m_Current.Angle.y, &m_Current.Forward, &m_Current.Upward);
	m_Current.Position = m_Current.Target - m_Current.Distance * m_Current.Forward;
}

// �����_���v�Z
void Camera::ComputeTarget()
{
	ToVector(m_Current.Angle.x, m_Current.Angle.y, &m_Current.Forward, &m_Current.Upward);
	m_Current.Target = m_Current.Position + m_Current.Distance * m_Current.Forward;
}

// ��]�p���v�Z
void Camera::ComputeAngle()
{
	m_Current.Forward = m_Current.Target - m_Current.Position;
	ToAngle(m_Current.Forward, &m_Current.Angle.x, &m_Current.Angle.y, &m_Current.Distance);
	ToVector(m_Current.Angle.x, m_Current.Angle.y, nullptr, &m_Current.Upward);
}

// ====== �v���W�F�N�^�N���X ======
Projector::Projector()
{
	m_Current.Mode = Mode::Perspective;
	m_Current.Aspect = 1.333f;
	m_Current.FieldOfView = DirectX::XM_PIDIV4;
	m_Current.NearClip = 1.0f;
	m_Current.FarClip = 1000.0f;
	m_Current.Left = 0.0f;
	m_Current.Right = 100.0f;
	m_Current.Top = 0.0f;
	m_Current.Bottom = 100.0f;

	m_Preserve = m_Current;
}

// �f�X�g���N�^
Projector::~Projector()
{
}

// ���݂̎ˉe�p�����[�^��ۑ�
void Projector::Preserve()
{
	m_Preserve = m_Current;
}

// �ˉe�p�����[�^�����Z�b�g
void Projector::Reset()
{
	m_Current = m_Preserve;
	switch (m_Current.Mode)
	{
	case Mode::Perspective:
	{
		m_Proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
			m_Current.FieldOfView,
			m_Current.Aspect,
			m_Current.NearClip,
			m_Current.FarClip);
	}
	break;

	case Mode::Orhographic:
	{
		m_Proj = DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(
			m_Current.Left,
			m_Current.Right,
			m_Current.Bottom,
			m_Current.Top,
			m_Current.NearClip,
			m_Current.FarClip);
	}
	break;
	}
}

// �������e�p�����[�^��ݒ�
void Projector::SetPerspective(float fov, float aspect, float nearClip, float farClip)
{
	m_Current.Mode = Mode::Perspective;
	m_Current.FieldOfView = fov;
	m_Current.Aspect = aspect;
	m_Current.NearClip = nearClip;
	m_Current.FarClip = farClip;

	m_Proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearClip, farClip);
}

// ���ˉe�p�����[�^��ݒ�
void Projector::SetOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip)
{
	m_Current.Mode = Mode::Orhographic;
	m_Current.Left = left;
	m_Current.Right = right;
	m_Current.Top = top;
	m_Current.Bottom = bottom;

	m_Proj = DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(left, right, bottom, top, nearClip, farClip);
}

// �ˉe���[�h���擾
const Projector::Mode& Projector::GetMode() const
{
	return m_Current.Mode;
}

// ������p���擾
const float& Projector::GetFieldOfView() const
{
	return m_Current.FieldOfView;
}

// �A�X�y�N�g����擾
const float& Projector::GetAspect() const
{
	return m_Current.Aspect;
}

// �߃N���b�v���ʂ܂ł̋������擾
const float& Projector::GetNearClip() const
{
	return m_Current.NearClip;
}

// ���N���b�v���ʂ܂ł̋������擾
const float& Projector::GetFarClip() const
{
	return m_Current.FarClip;
}

// �ˉe�s����擾
const DirectX::SimpleMath::Matrix& Projector::GetMatrix() const
{
	return m_Proj;
}