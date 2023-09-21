#pragma once
#include "Core/MathDefinitions.h"

class Camera
{
public:
	Camera() : m_fov(0.f), m_nearPlane(0.f), m_farPlane(0.f), m_aspectRatio(0.f), m_renderTarget(0) {}
	Camera(float fov, float aspectRatio, float nearPlane = 0.000001f, float farPlane = 1000.f);
	~Camera();
	
	const Mat4f& GetProjection() { return m_projection; }

	void SetFov(float fov);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);
	void SetAspectRatio(float aspectRatio);

private:
	friend class Editor;
	friend class Renderer;
	void ConstructProjection();

	UINT m_renderTarget;

	float m_fov;
	float m_nearPlane;
	float m_farPlane;
	float m_aspectRatio;

	Mat4f m_projection;
};

