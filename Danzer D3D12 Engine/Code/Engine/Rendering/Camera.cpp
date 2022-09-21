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
	float radiansFOV   = m_fov * (3.14f / 180.f);
	m_projection(0, 0) = (1 / tanf(radiansFOV / 2.0f));
	m_projection(1, 1) = (m_aspectRatio) * (1 / tanf(radiansFOV / 2.0f));
	m_projection(2, 2) =  m_farPlane / (m_farPlane - m_nearPlane);
	m_projection(3, 2) = -m_nearPlane * m_farPlane / (m_farPlane - m_nearPlane);
	m_projection(2, 3) = 1;
	m_projection(3, 3) = 0;
}
