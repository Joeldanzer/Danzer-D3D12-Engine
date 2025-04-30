#include "stdafx.h"
#include "SceneManager.h"

#include "Core/RegistryWrapper.h"

#include "Components/AllComponents.h"
#include "Rendering/Camera.h"

#include <fstream>
#include <iostream>
#include <filesystem>

SceneManager::SceneManager()
{
	Entity entity = REGISTRY->Create3DEntity("MainCamera");
	Camera& cam = REGISTRY->Emplace<Camera>(entity);

	cam.SetCameraProjection(Camera::PERSPECTIVE);
	cam.SetFov(65.0f);
	cam.SetAspectRatio(WindowHandler::WindowData().AspectRatio());
	cam.SetNearPlane(0.01f);
	cam.SetFarPlane(1000.0f);

	Transform& transform = REGISTRY->Get<Transform>(entity);
	transform.m_position = { 0.f, 10.0f, 0.f };
	m_mainCamera = entity;

	Entity dirLight = Reg::Instance()->Create3DEntity("DirectionalLight");
	REGISTRY->Emplace<DirectionalLight>(dirLight, DirectionalLight(
		{ 255.0f / 255.0f, 214.0f / 255.f, 165.f / 255.f, 4.0f },
		{ 1.0f, 1.0f, 1.0f, 0.25f }));
	Transform& lightTransform  = Reg::Instance()->Get<Transform>(dirLight);
	lightTransform.m_rotation  = Quat4f::CreateFromAxisAngle(Vect3f::Right, ToRadians(70.0f));
	lightTransform.m_rotation *= Quat4f::CreateFromAxisAngle(Vect3f::Up,    ToRadians(0.0f));
	lightTransform.m_position  = { 0.0f, 0.0f, 0.0f };
}

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

}

void SceneManager::UpdateLastPositions()
{

}
