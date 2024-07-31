// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [MyMath.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/26
// 概要   : 数学関連の処理をまとめたクラス
// 更新履歴
// 2024/06/26  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "SimpleMath.h"		// DirectXMathのラッパー
using namespace DirectX::SimpleMath;

// トランスフォーム
struct Transform
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 scale;
	Transform(DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::Identity, DirectX::SimpleMath::Vector3 sca = DirectX::SimpleMath::Vector3::One)
		: position(pos), rotation(rot), scale(sca) {}

	// ワールド行列の取得
	inline DirectX::SimpleMath::Matrix GetWorldMatrix()
	{
		DirectX::SimpleMath::Matrix mat = DirectX::SimpleMath::Matrix::CreateScale(scale);
		mat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		mat *= DirectX::SimpleMath::Matrix::CreateTranslation(position);
		return mat;
	}

	// ローカル行列の取得
	inline DirectX::SimpleMath::Matrix GetLocalMatrix()
	{
		DirectX::SimpleMath::Matrix mat = DirectX::SimpleMath::Matrix::CreateScale(scale);
		mat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		return mat;
	}
};

inline physx::PxMat44 ToPxMat44(const DirectX::SimpleMath::Matrix& mat) {
	return physx::PxMat44(
		physx::PxVec4(mat._11, mat._12, mat._13, mat._14),
		physx::PxVec4(mat._21, mat._22, mat._23, mat._24),
		physx::PxVec4(mat._31, mat._32, mat._33, mat._34),
		physx::PxVec4(mat._41, mat._42, mat._43, mat._44)
	);
}

inline physx::PxVec3 ToPxVec3(const DirectX::SimpleMath::Vector3& vec3) {
	return physx::PxVec3(vec3.x, vec3.y, vec3.z);
}

inline physx::PxVec4 ToPxVec4(const DirectX::SimpleMath::Vector4& vec4) {
	return physx::PxVec4(vec4.x, vec4.y, vec4.z, vec4.w);
}

inline physx::PxQuat ToPxQuat(const DirectX::SimpleMath::Quaternion& quat) {
	return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}

inline physx::PxTransform ToPxTransform(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& rotation) {
	return physx::PxTransform(ToPxVec3(position), ToPxQuat(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation * (DirectX::XM_PI / 180.0f))));
}
inline physx::PxTransform ToPxTransform(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& quaternion) {
	return physx::PxTransform(ToPxVec3(position), ToPxQuat(quaternion));
}

inline physx::PxTransform ToPxTransform(const Transform& transform) {
	return physx::PxTransform(ToPxVec3(transform.position), ToPxQuat(transform.rotation));
}

/* PhysX math to DirectX Simple Math */

inline DirectX::SimpleMath::Matrix ToMatrix(const physx::PxMat44& px_mat44) {
	return DirectX::SimpleMath::Matrix(
		px_mat44.column0.x, px_mat44.column0.y, px_mat44.column0.z, px_mat44.column0.w,
		px_mat44.column1.x, px_mat44.column1.y, px_mat44.column1.z, px_mat44.column1.w,
		px_mat44.column2.x, px_mat44.column2.y, px_mat44.column2.z, px_mat44.column2.w,
		px_mat44.column3.x, px_mat44.column3.y, px_mat44.column3.z, px_mat44.column3.w
	);
}

inline DirectX::SimpleMath::Vector3 ToVector3(const physx::PxVec3& px_vec3) {
	return DirectX::SimpleMath::Vector3(px_vec3.x, px_vec3.y, px_vec3.z);
}

inline DirectX::SimpleMath::Vector4 ToVector4(const physx::PxVec4& px_vec4) {
	return DirectX::SimpleMath::Vector4(px_vec4.x, px_vec4.y, px_vec4.z, px_vec4.w);
}

inline DirectX::SimpleMath::Quaternion ToQuaternion(const physx::PxQuat& px_quat) {
	return DirectX::SimpleMath::Quaternion(px_quat.x, px_quat.y, px_quat.z, px_quat.w);
}

inline Transform ToTransform(const physx::PxTransform& px_transform) {
	return Transform(ToVector3(px_transform.p), ToQuaternion(px_transform.q));
}


