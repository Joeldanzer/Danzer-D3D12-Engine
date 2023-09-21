#include "Editor.h"

#include "Core/Engine.h"
#include "Core/input.hpp"
#include "Core/WindowHandler.h"

#include "SceneManager.h"

#include "Components/Transform.h"
#include "Rendering/Camera.h"

#include "Components/Model.h"
#include "Rendering/Models/ModelHandler.h"

Editor::Editor(Engine& engine) :
	m_engine(engine),
	m_turnSpeed(50.f),
	m_moveSpeed(10.f),
	m_imguiHandler(engine)
{
	m_imguiHandler.Init();
}
Editor::~Editor(){}

void Editor::Update(const float deltaTime)
{
	CameraControlls(deltaTime);
	m_imguiHandler.Update(deltaTime);
}

void Editor::CameraControlls(const float dt)
{
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();
	Transform& transform = reg.get<Transform>(m_engine.GetSceneManager().GetCurrentScene().GetMainCamera());
	Camera& camera = reg.get<Camera>(m_engine.GetSceneManager().GetCurrentScene().GetMainCamera());

	Vect3f euler = transform.m_rotation.ToEuler();

	euler = { ToDegrees(euler.x), ToDegrees(euler.y), ToDegrees(euler.z) };

	if (Input::GetInstance().IsKeyDown(VK_SHIFT)) {
		if (Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaX() > 0 ||
			Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaX() < 0) {
			euler.y += Input::GetInstance().MouseDeltaX() * m_turnSpeed * dt; 
		}

		if (Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaY() > 0 ||
			Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaY() < 0) {
			euler.x += Input::GetInstance().MouseDeltaY() * m_turnSpeed * dt;
		}
	}

	transform.m_rotation = Quat4f::CreateFromYawPitchRoll({ToRadians(euler.x), ToRadians(euler.y), ToRadians(euler.z)});
	
	Vect3f forward = transform.World().Forward();
	if (Input::GetInstance().MouseWheel() < 0){
		transform.m_position += (forward * m_moveSpeed) * dt;
	}
	else if(Input::GetInstance().MouseWheel() > 0)
	{
		transform.m_position -= (forward * m_moveSpeed) * dt;
	}
	
	if (Input::GetInstance().IsKeyPressed('Z')) {
		camera.m_renderTarget++;
		
		if (camera.m_renderTarget > 5) {
			camera.m_renderTarget = 0;
		}
	}

}
