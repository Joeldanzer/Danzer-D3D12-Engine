#include "stdafx.h"
#include "SceneManager.h"

#include "Components/Light/DirectionalLight.h"
#include "Components/Transform.h"

void SceneManager::Init(Camera& cam)
{
	// Create a default empty scene that the games start with.
	Scene& scene = CreateEmptyScene("default");

	auto entity = scene.CreateBasicEntity("MainCamera").m_entity;
	scene.Registry().emplace<Camera>(entity, cam);

	Transform& transform = scene.Registry().get<Transform>(entity);
	transform.m_position = { 0.f, 10.0f, 0.f };

	SetScene("default", entity, false);
	auto dirLight = scene.CreateBasicEntity("DirectionalLight").m_entity;
	scene.Registry().emplace<DirectionalLight>(dirLight, DirectionalLight(
		{ 255.0f/255.0f, 214.0f/255.f, 165.f/255.f, 4.0f}, 
		{1.0f, 1.0f, 1.0f, 0.25f}));
	//scene.Registry().get<DirectionalLight>(dirLight).SetShadowProjection(128.0f, 1.0f);
	Transform& lightTransform  = scene.Registry().get<Transform>(dirLight);
	lightTransform.m_rotation  = Quat4f::CreateFromAxisAngle(Vect3f::Right,  ToRadians(70.0f));//Quat4f::CreateFromYawPitchRoll({ -65.0f, 40.0f, 0.0f });
	lightTransform.m_rotation *= Quat4f::CreateFromAxisAngle(Vect3f::Up,  ToRadians(0.0f));//Quat4f::CreateFromYawPitchRoll({ -65.0f, 40.0f, 0.0f });
	
	lightTransform.m_position = { 0.0f, 0.0f, 0.0f };
}

Scene& SceneManager::CreateEmptyScene(std::string sceneName)
{
	Scene scene;
	m_scenes.emplace(sceneName, scene);
	
	return m_scenes[sceneName];
}

void SceneManager::AddScene(std::string name, const Scene& scene)
{
	m_scenes.emplace(name, scene);
}

bool SceneManager::SetScene(std::string name, entt::entity camera, bool resetScene)
{
	if (m_scenes.find(name) != m_scenes.end()) {

		m_currentScene = name;
		m_scenes[name].m_mainCamera = camera;
		if (resetScene)
			m_scenes[name].ResetAllObjectsInScene();

		return true;
	}

	return false;
}

GameEntity& SceneManager::CreateBasicEntity(std::string name, bool isStatic, GameEntity::STATE state)
{
	return m_scenes[m_currentScene].CreateBasicEntity(name, isStatic, state);
}
