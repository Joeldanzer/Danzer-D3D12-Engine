#include "stdafx.h"
#include "SceneManager.h"

#include "Components/DirectionalLight.h"
#include "Components/Transform.h"

void SceneManager::Init(Camera& cam)
{
	// Create a default empty scene that the games start with.
	Scene& scene = CreateEmptyScene("default");

	auto entity = scene.CreateBasicEntity("MainCamera");
	scene.Registry().emplace<Camera>(entity, cam);

	Transform& transform = scene.Registry().get<Transform>(entity);
	transform.m_position = { 0.f, 2.f, -5.f };

	SetScene("default", entity, false);
	entt::entity dirLight = scene.CreateBasicEntity("DirectionalLight", "Light");
	scene.Registry().emplace<DirectionalLight>(dirLight, DirectionalLight({ 1.f, 1.f, 1.f, 1.f }, {1.f, 1.0f, 1.f, 0.5f}));
	Transform& lightTransform = scene.Registry().get<Transform>(dirLight);	
	lightTransform.m_rotation = Quat4f::CreateFromYawPitchRoll({ 0.f, 0.f, 180.f });
}

//void SceneManager::CreateLoadedScene(LevelScene* loadedScene, std::string sceneName)
//{
//	if (sceneName.empty())
//		CreateEmptyScene(loadedScene->m_sceneName);
//	else
//		CreateEmptyScene(sceneName);
//}

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
