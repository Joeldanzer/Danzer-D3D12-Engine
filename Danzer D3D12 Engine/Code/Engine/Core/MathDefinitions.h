#pragma once
#include "../3rdParty/DirectXTK12-main/Inc/SimpleMath.h"
#include <cmath>

typedef DirectX::SimpleMath::Vector2 Vect2f;
typedef DirectX::SimpleMath::Vector3 Vect3f;
typedef DirectX::SimpleMath::Vector4 Vect4f;

typedef DirectX::SimpleMath::Quaternion Quat4f;

typedef DirectX::SimpleMath::Matrix Mat4f;

using namespace DirectX::SimpleMath;

inline float ToRadians(const float degree) {
	return degree * (3.14f / 180.f);
}
inline float ToDegrees(const float radian) {
	return radian * (180.f / 3.14f);
}

inline float DistanceVect3(Vect3f p1, Vect3f p2) {
	return sqrtf(
				((p2.x - p1.x) * (p2.x - p1.x)) + 
				((p2.y - p1.y) * (p2.y - p1.y)) + 
				((p2.z - p1.z) * (p2.z - p1.z))
	);
}
inline float Lerp(float a, float b, float t) {
	return a + t * (b - a);
}
inline Vect3f Lerp(Vect3f p1, Vect3f p2, float t) {
	Vect3f lerpVector = {
		Lerp(p1.x, p2.x, t), Lerp(p1.y, p2.y, t), Lerp(p1.z, p2.z, t)
	};
	return lerpVector;
}
inline Vect3f DirectionBetween2Points(Vect3f v1, Vect3f v2) {
	Vect3f dir = v1 - v2;
	dir.Normalize();
	return dir;
}
inline Vect4f Mul(Mat4f mat, Vect4f pos) {
	return {
		pos.x * mat._11 + pos.y * mat._21 + pos.z * mat._31 + pos.w * mat._41,
		pos.x * mat._12 + pos.y * mat._22 + pos.z * mat._32 + pos.w * mat._42,
		pos.x * mat._13 + pos.y * mat._23 + pos.z * mat._33 + pos.w * mat._43,
		pos.x * mat._14 + pos.y * mat._24 + pos.z * mat._34 + pos.w * mat._44,
		//pos.x* mat._11 + pos.y * mat._12 + pos.z * mat._13 + pos.w * mat._14,
		//pos.x* mat._21 + pos.y * mat._22 + pos.z * mat._23 + pos.w * mat._24,
		//pos.x* mat._31 + pos.y * mat._32 + pos.z * mat._33 + pos.w * mat._34,
		//pos.x* mat._41 + pos.y * mat._42 + pos.z * mat._43 + pos.w * mat._44,
	};
}

//inline float YawFromQuat(Quat4f quat) {
//
//}
//
//inline float PitchFromQuat(Quat4f quat) {
//
//}
//
//inline float RollFromQuat(Quat4f quat) {
//
//}