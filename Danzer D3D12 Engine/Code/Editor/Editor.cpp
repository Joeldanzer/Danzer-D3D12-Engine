#include "EditorPCH.h"

#include "Editor.h"

#include "Core/Engine.h"
#include "Core/input.hpp"
#include "Core/WindowHandler.h"

#include "SceneManager.h"

#include "Components/Transform.h"
#include "Rendering/Camera.h"

#include "Components/Model.h"
#include "Rendering/Models/ModelHandler.h"

Editor* Editor::s_instance = nullptr;

Editor::Editor() :
	m_turnSpeed(3.f),
	m_moveSpeed(8.f),
	m_imguiHandler()
{
	m_editorCam			 = new Camera();
	m_editorCamTransform = new Transform(); 

	m_editorCam->SetFov(75.0f);
	m_editorCam->SetAspectRatio(WindowHandler::GetViewPort().Width / WindowHandler::GetViewPort().Height);
	m_editorCam->SetFarZ(10000.0f);
	m_editorCam->SetNearZ(0.01f);

	m_editorCamTransform->m_position = { 0.0f, 2.0f, 10.0f };

	m_imguiHandler.Init();
}
Editor::~Editor(){
	delete m_editorCam;
	delete m_editorCamTransform;
}

void Editor::Update(const float deltaTime)
{
	CameraControlls(deltaTime);
	m_imguiHandler.Update(deltaTime);
}

void Editor::CameraControlls(const float dt)
{
	if (Input::GetInstance().IsKeyDown(VK_LEFT))
		m_editorCamTransform->m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f::Up, dt * m_turnSpeed);
	if (Input::GetInstance().IsKeyDown(VK_RIGHT))
		m_editorCamTransform->m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f::Up, -(dt * m_turnSpeed));

	if (Input::GetInstance().IsKeyDown(VK_DOWN))
		m_editorCamTransform->m_rotation *= DirectX::XMQuaternionRotationAxis(m_editorCamTransform->World().Right(), -(dt * m_turnSpeed));
	if (Input::GetInstance().IsKeyDown(VK_UP))
		m_editorCamTransform->m_rotation *= DirectX::XMQuaternionRotationAxis(m_editorCamTransform->World().Right(), dt * m_turnSpeed);

	Vector3 forward = m_editorCamTransform->World().Forward();

	if (Input::GetInstance().IsKeyDown('W'))
		m_editorCamTransform->m_position += (m_editorCamTransform->World().Forward() * m_moveSpeed) * dt;
	if (Input::GetInstance().IsKeyDown('S'))
		m_editorCamTransform->m_position -= (m_editorCamTransform->World().Forward() * m_moveSpeed) * dt;

	if (Input::GetInstance().IsKeyDown('A'))
		m_editorCamTransform->m_position -= (m_editorCamTransform->World().Right() * m_moveSpeed) * dt;
	if (Input::GetInstance().IsKeyDown('D'))
		m_editorCamTransform->m_position += (m_editorCamTransform->World().Right() * m_moveSpeed) * dt;

	if (Input::GetInstance().IsKeyDown(VK_SPACE))
		m_editorCamTransform->m_position.y += dt * m_moveSpeed;
	if (Input::GetInstance().IsKeyDown(VK_SHIFT))
		m_editorCamTransform->m_position.y -= dt * m_moveSpeed;

	if (Input::GetInstance().IsKeyPressed('Z')) {
			m_editorCam->RenderTarget() = m_editorCam->RenderTarget() < 9 ? m_editorCam->RenderTarget() + 1 : 0;	
	}
}
