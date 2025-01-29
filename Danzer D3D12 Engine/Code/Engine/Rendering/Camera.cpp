#include "stdafx.h"
#include "Camera.h"
#include "Core/WindowHandler.h"

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	m_projection = Mat4f();

	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;

	m_renderTarget = 0;

	ConstructProjection();
}
Camera::~Camera(){}

void Camera::SetFov(float fov)
{
	m_fov = fov;
	ConstructProjection();
}

void Camera::SetNearPlane(float nearPlane)
{
	m_nearPlane = nearPlane;
	ConstructProjection();
}

void Camera::SetFarPlane(float farPlane)
{
	m_farPlane = farPlane;
	ConstructProjection();
}

void Camera::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
	ConstructProjection();
}

void Camera::ConstructProjection()
{
	float radiansFOV = m_fov * (3.14f / 180.f);
	m_projection     = DirectX::XMMatrixPerspectiveFovRH(radiansFOV, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::ConstructFrustrum(const Mat4f& transform, const Vect3f& position)
{
	Mat4f  m   = transform;
	Vect3f pos = m.Translation();
	//m_frustrum[LEFT_FACE]   = Planef((m._14 + m._11), (m._24 + m._21), (m._34 + m._31), (m._44 + m._41));
	//m_frustrum[RIGHT_FACE]  = Planef((m._14 - m._11), (m._24 - m._21), (m._34 - m._31), (m._44 - m._41));
	//m_frustrum[BOTTOM_FACE] = Planef((m._14 + m._12), (m._24 + m._22), (m._34 + m._32), (m._44 + m._42));
	//m_frustrum[TOP_FACE]    = Planef((m._14 - m._12), (m._24 - m._22), (m._34 - m._32), (m._44 - m._42));
	//m_frustrum[NEAR_FACE]   = Planef((m._13),		  (m._23),		   (m._33),			(m._43));
	//m_frustrum[FAR_FACE]    = Planef((m._14 - m._13), (m._24 - m._23), (m._34 - m._32), (m._44 - m._43));
	//
	//for (uint16_t i = 0; i < Faces::FACE_COUNT; i++)
	//	m_frustrum[i].Normalize();
	
	const float  fovY		  = tanf(ToRadians(m_fov) * 0.5f);
	const float  halfVSide    = m_farPlane * fovY;
	const float  halfHSide    = halfVSide  * m_aspectRatio;
	const Vect3f frontMultFar = m_nearPlane * m.Forward();
	
	m_frustrum[NEAR_FACE] = Planef(pos + m_nearPlane * m.Forward(), -m.Forward());
	m_frustrum[FAR_FACE]  = Planef(pos + frontMultFar,			     m.Forward());
	
	Vect3f left = frontMultFar + m.Right() * halfHSide;
	left = left.Cross(m.Up());
	left.Normalize();
	m_frustrum[LEFT_FACE] = Planef(pos, left);
	
	Vect3f right = m.Up();
	right = right.Cross(frontMultFar - m.Right() * halfHSide);
	right.Normalize();
	m_frustrum[RIGHT_FACE] = Planef(pos, right);
	
	Vect3f up = m.Right();
	up = up.Cross(frontMultFar + m.Up() * halfVSide);
	up.Normalize();
	m_frustrum[BOTTOM_FACE] = Planef(pos, up);
	
	Vect3f bot = frontMultFar - m.Up() * halfVSide;
	bot = bot.Cross(m.Right());
	bot.Normalize();
	m_frustrum[TOP_FACE] = Planef(pos, bot);
}
