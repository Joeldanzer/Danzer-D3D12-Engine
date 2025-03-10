#include "stdafx.h"
#include "SceneManager.h"

#include "Core/RegistryWrapper.h"

#include "Components/AllComponents.h"
#include "Rendering/Camera.h"

#include <fstream>
#include <iostream>
#include <filesystem>

SceneManager::SceneManager(Camera& cam)
{
	cam = Camera();
	cam.SetCameraProjection(Camera::PERSPECTIVE);
	cam.SetFov(65.0f);
	cam.SetAspectRatio(WindowHandler::WindowData().AspectRatio());
	cam.SetNearPlane(0.01f);
	cam.SetFarPlane(10000.0f);

	// Look up a scene in our bin folder, if it can't find any we create a new one.
	
	//auto entity = CreateBasicEntity("MainCamera", false).m_entity;
	Entity entity = REGISTRY->Create3DEntity("MainCamera");
	REGISTRY->Emplace<Camera>(entity, cam);
	Transform& transform = Reg::Instance()->Get<Transform>(entity);
	transform.m_position = { 0.f, 10.0f, 0.f };
	m_mainCamera = entity;

	Entity dirLight = Reg::Instance()->Create3DEntity("DirectionalLight");
	Reg::Instance()->Emplace<DirectionalLight>(dirLight, DirectionalLight(
		{ 255.0f / 255.0f, 214.0f / 255.f, 165.f / 255.f, 4.0f },
		{ 1.0f, 1.0f, 1.0f, 0.25f }));
	Transform& lightTransform  = Reg::Instance()->Get<Transform>(dirLight);
	lightTransform.m_rotation  = Quat4f::CreateFromAxisAngle(Vect3f::Right, ToRadians(70.0f));
	lightTransform.m_rotation *= Quat4f::CreateFromAxisAngle(Vect3f::Up,    ToRadians(0.0f));
	lightTransform.m_position  = { 0.0f, 0.0f, 0.0f };
}

//Scene& SceneManager::CreateEmptyScene(std::string sceneName)
//{
//	Scene scene(sceneName);
//	m_scenes.emplace(sceneName, scene);
//	
//	return m_scenes[sceneName];
//}

//void SceneManager::AddScene(std::string name, const Scene& scene)
//{
//	m_scenes.emplace(name, scene);
//}

bool SceneManager::SetScene(std::string name, entt::entity camera, bool resetScene)
{
	//if (m_scenes.find(name) != m_scenes.end()) {
	//
	//	m_currentScene = name;
	//	m_scenes[name].m_mainCamera = camera;
	//	if (resetScene)
	//		m_scenes[name].ResetAllObjectsInScene();
	//
	//	return true;
	//}
	//
	//return false;

	return false;
}

void SceneManager::SearchForExistingScenes()
{
	std::string path("");
	std::string ext(".sce");
	for (auto &p  : std::filesystem::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ext) {

		}
	}
}

void SceneManager::UpdateTransformsForRendering(bool updateStaticObjects)
{
	//auto view = m_registry.view<Transform, GameEntity>();
	//for (auto entity : view)
	//{
	//	const GameEntity& gameEntt = view.get<GameEntity>(entity);
	//	//if(!updateStaticObjects)
		//	if (gameEntt.m_static || gameEntt.m_state != GameEntity::STATE::ACTIVE)
		//		continue;
	//
	//
	//	Transform& transform = view.get<Transform>(entity);
	//	const Vector3& pos   = transform.m_position;
	//	const Vector3& scale = transform.m_scale;
	//
	//	transform.m_last		 = transform.m_world;
	//	transform.m_lastPosition = transform.m_world.Translation();
	//
	//	Mat4f mat;
	//	DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&transform.m_rotation);
	//	mat *= DirectX::XMMatrixRotationQuaternion(quatv);
	//	mat *= DirectX::XMMatrixScaling(transform.m_scale.x, transform.m_scale.y, transform.m_scale.z);
	//	mat *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	//
	//	transform.m_local = mat;
	//	transform.m_world = !transform.Parent() ? transform.m_world = transform.m_local : transform.m_local * transform.Parent()->m_world;
	//
	//	transform.m_lastPosition = pos;
	//}
	//
	//const Transform& camTransform = m_registry.get<Transform>(m_mainCamera);

	//auto lightView = m_registry.view<Transform, DirectionalLight>();
	//for (auto entity : lightView )
	//{
	//	Transform& transform = m_registry.get<Transform>(entity);
	//	transform.m_position.x = camTransform.m_position.x;
	//	transform.m_position.y = camTransform.m_position.z;
	//}
}

void SceneManager::UpdateLastPositions()
{
	//auto view = m_registry.view<Transform, GameEntity>();
	//for (auto entity : view)
	//{
	//	const GameEntity& gameEntt = view.get<GameEntity>(entity);
	//	if (gameEntt.m_static || gameEntt.m_state != GameEntity::STATE::ACTIVE)
	//		continue;
	//
	//	Transform& transform = view.get<Transform>(entity);
	//	transform.m_lastPosition = transform.m_position;
	//}
}
