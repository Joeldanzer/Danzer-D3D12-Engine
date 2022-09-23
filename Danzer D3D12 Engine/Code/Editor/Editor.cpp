#include "Editor.h"

#include "Core/Engine.h"
#include "Core/input.hpp"

#include "SceneManager.h"

#include "Components/Transform.h"
#include "Rendering/Camera.h"

#include "Components/Model.h"
#include "Rendering/Models/ModelHandler.h"

Editor::Editor(Engine& engine) :
	m_engine(engine),
	m_turnSpeed(2.f),
	m_moveSpeed(10.f)
{
	m_imguiHandler.Init(engine);
	
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();
	Scene& scene = engine.GetSceneManager().GetCurrentScene();

	entt::entity entity = scene.CreateBasicEntity("cube");
	Model model(engine.GetModelHandler().CreateCustomModel(ModelData::GetCube()));
	reg.emplace<Model>(entity, model);

	Transform& transform = reg.get<Transform>(entity);
	transform.m_scale = { 0.1f, 0.1f, 0.1f };

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

	if (Input::GetInstance().IsKeyDown(VK_SHIFT)) {
		if (Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaX() > 0 || 
			Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaX() < 0) {
			transform.m_rotation *= Quat4f::CreateFromAxisAngle({0.f, 1.f, 0.f}, (m_turnSpeed * Input::GetInstance().MouseDeltaX()) * dt);
		}

		if (Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaY() > 0 ||
			Input::GetInstance().IsMouseDown(Input::MouseButton::Left) && Input::GetInstance().MouseDeltaY() < 0) {
			transform.m_rotation *= Quat4f::CreateFromAxisAngle(transform.GetWorld().Right(), (m_turnSpeed * Input::GetInstance().MouseDeltaY()) * dt);
		}
	}
	
	transform.m_rotation.z = 0.f;
	
	Vect3f forward = transform.GetWorld().Forward();
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
