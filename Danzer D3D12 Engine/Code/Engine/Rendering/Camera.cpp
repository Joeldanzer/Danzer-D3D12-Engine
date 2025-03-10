#include "stdafx.h"
#include "Camera.h"
#include "Core/WindowHandler.h"

Camera::Camera() :
	m_fov(65.0f), m_nearPlane(0.01f), m_farPlane(1000.f), m_aspectRatio(16.0f / 9.0f), m_renderTarget(0),
	m_viewWidth(10.0f), m_viewHeight(10.0f), m_nearZ(-10.0f), m_farZ(10.0f),
	m_currentProj(PERSPECTIVE)
{
	m_projection[PERSPECTIVE]  = Mat4f();
	m_projection[ORTHOGRAPHIC] = Mat4f();

	ConstructOrthographic();
	ConstructPerspective();
}
Camera::~Camera(){}

void Camera::SetFov(float fov)
{
	if (m_fov != fov) {
		m_fov = fov;
		ConstructPerspective();
	}
}

void Camera::SetNearPlane(float nearPlane)
{
	if (m_nearPlane != nearPlane) {
		m_nearPlane = nearPlane;
		ConstructPerspective();
	}
}

void Camera::SetFarPlane(float farPlane)
{
	if (m_farPlane != farPlane) {
		m_farPlane = farPlane;
		ConstructPerspective();
	}
}

void Camera::SetAspectRatio(float aspectRatio)
{
	if (m_aspectRatio != aspectRatio) {
		m_aspectRatio = aspectRatio;
		ConstructPerspective();
	}
}

void Camera::SetViewWidth(float width)
{
	if (m_viewWidth != width) {
		m_viewWidth = width;
		ConstructOrthographic();
	}
}

void Camera::SetViewHeight(float height)
{
	if (m_viewHeight != height) {
		m_viewHeight = height;
		ConstructOrthographic();
	}
}

void Camera::SetNearZ(float nearZ)
{
	if (m_nearZ != nearZ) {
		m_nearZ = nearZ;
		ConstructOrthographic();
	}
}

void Camera::SetFarZ(float farZ)
{
	if (m_farZ != farZ) {
		m_farZ = farZ;
		ConstructOrthographic();
	}
}

void Camera::ConstructPerspective()
{
	float radiansFOV = m_fov * (3.14f / 180.f);
	m_projection[PERSPECTIVE] = DirectX::XMMatrixPerspectiveFovRH(radiansFOV, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::ConstructOrthographic()
{
	m_projection[ORTHOGRAPHIC] = DirectX::XMMatrixOrthographicRH(m_viewWidth, m_viewHeight, m_nearZ, m_farZ);
}

void Camera::ConstructFrustrum(const Mat4f& transform, const Vect3f& position)
{
	Mat4f  m   = transform;
	Vect3f pos = m.Translation();

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

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void Camera::DisplayInEditor(const Entity entity){
	Camera& cam = REGISTRY->Get<Camera>(entity);

	const std::string projections[2] = { "Perspective", "Orthographic" };
	if (ImGui::BeginCombo("Projection", projections[cam.m_currentProj].c_str())) {
		for (uint8_t i = 0; i < _countof(projections); i++)
		{
			bool selected = false;
			ImGui::Selectable(projections[i].c_str(), &selected);
			if (selected) {
				cam.m_currentProj = (Projection)i;
			}
		}
		ImGui::EndCombo();
	}

	if (cam.m_currentProj == PERSPECTIVE) {
		float fov = cam.m_fov;
		ImGui::DragFloat("Field Of View", &fov, 1.0f, 1.0f, 180.0f);

		float nearPlane = cam.m_nearPlane;
		ImGui::DragFloat("Near Plane", &nearPlane, 0.001f, 0.001f, 10.0f);
		
		float farPlane = cam.m_farPlane;
		ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 1.0f, 50000.0f);

		cam.SetFov(fov);
		cam.SetNearPlane(nearPlane);
		cam.SetFarPlane(farPlane);
	}

	if (cam.m_currentProj == ORTHOGRAPHIC) {
		float width = cam.m_viewWidth;
		ImGui::DragFloat("View Width", &width, 1.0f, 1.0f, 1000.0f);

		float height = cam.m_viewHeight;
		ImGui::DragFloat("View Height", &height, 1.0f, 1.0f, 1000.0f);
	
		float nearZ = cam.m_nearZ;
		ImGui::DragFloat("Near Z", &nearZ, 1.0f, -1000.0f, 1000.0f);

		float farZ = cam.m_farZ;
		ImGui::DragFloat("Far Z", &farZ, 1.0f, -1000.0f, 1000.0f);

		cam.SetViewWidth(width);
		cam.SetViewHeight(height);
		cam.SetNearZ(nearZ);
		cam.SetFarZ(farZ);
	}
}
#else
void Camera::DisplayInEditor(const Entity entity){}
#endif