#pragma once
#include "DirectXTK12/Inc/SimpleMath.h"
#include <cmath>

typedef DirectX::SimpleMath::Color   Colorf;
typedef DirectX::SimpleMath::Vector2 Vect2f;
typedef DirectX::SimpleMath::Vector3 Vect3f;
typedef DirectX::SimpleMath::Vector4 Vect4f;
typedef DirectX::SimpleMath::Plane   Planef;

typedef DirectX::SimpleMath::Quaternion Quat4f;

typedef DirectX::SimpleMath::Matrix Mat4f;

using namespace DirectX::SimpleMath;

inline float ToRadians(const float degree) {
	return degree * (3.14f / 180.f);
}
inline float ToDegrees(const float radian) {
	return radian * (180.f / 3.14f);
}

inline float DistanceVect3(const Vect3f& p1, const Vect3f& p2) {
	return sqrtf(
				((p2.x - p1.x) * (p2.x - p1.x)) + 
				((p2.y - p1.y) * (p2.y - p1.y)) + 
				((p2.z - p1.z) * (p2.z - p1.z))
	);
}
inline float Lerp(const float a, const float b, const float t) {
	return a + t * (b - a);
}
inline Vect3f Lerp(const Vect3f& p1, const Vect3f& p2, float t) {
	Vect3f lerpVector = {
		Lerp(p1.x, p2.x, t), Lerp(p1.y, p2.y, t), Lerp(p1.z, p2.z, t)
	};
	return lerpVector;
}
inline Vect3f DirectionBetween2Points(const Vect3f& v1, const Vect3f& v2) {
	Vect3f dir = v1 - v2;
	dir.Normalize();
	return dir;
}
inline Vect4f Mul(const Mat4f& mat, const Vect4f& pos) {
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
inline const float Dot(const Vect3f& v1, const Vect3f& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.y;
}
inline const Vect3f Cross(const Vect3f& v1, const Vect3f& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y - v2.x
	};
}
inline const Vect3f PlaneIntersecting(const Planef& p1, const Planef& p2, const Planef& p3) {
	Vect3f n1 = p1.Normal(), n2 = p2.Normal(), n3 = p3.Normal();

	if (n1.Dot(n2) >= 1) return Vect3f::Zero;
	if (n2.Dot(n3) >= 1) return Vect3f::Zero;
	if (n3.Dot(n1) >= 1) return Vect3f::Zero;

	float d1 = p1.D(), d2 = p2.D(), d3 = p3.D();

	Vect3f numerator = d1 * (n2.Cross(n3)) + d2 * (n3.Cross(n1)) + d3 * (n1.Cross(n2));
	float  denominator = (n2.Cross(n1)).Dot(n3);
	Vect3f intersection = numerator / denominator;

	return intersection;
}

inline const Mat4f Matrix4Abs(const Mat4f& mat) {
	Mat4f absMat = Mat4f::Identity;
	for (uint32_t y = 0; y < 4; y++)
		for (uint32_t x = 0; x < 4; x++)
			absMat.m[y][x] = std::fabs(mat.m[y][x]);
		
	return absMat;
}

// Quaternion function for rotating euler angles values correctly in editor.
inline const Quat4f EditorQuatRotate(const Quat4f& currentQuat, const Vect3f& newRotation, const Vect3f& lastRotation) {
	Vect3f deltaRot = newRotation - lastRotation;

	Quat4f deltaQuat = DirectX::XMQuaternionRotationRollPitchYaw(ToRadians(deltaRot.x), ToRadians(deltaRot.y), ToRadians(deltaRot.z));
	Quat4f newQuat   = DirectX::XMQuaternionMultiply(currentQuat, deltaQuat);

	return newQuat;
}

inline const Vect3f RadiansVectorToDegrees(const Vect3f& radiansVec) {	
	return { ToDegrees(radiansVec.x), ToDegrees(radiansVec.y), ToDegrees(radiansVec.z) };
}