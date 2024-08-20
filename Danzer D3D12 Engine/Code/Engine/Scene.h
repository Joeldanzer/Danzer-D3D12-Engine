#pragma once
#include "../3rdParty/entt/entt.hpp"
#include "Components/GameEntity.h"

class Scene
{
public:
	Scene();
	Scene(const Scene& scene) {}
	~Scene();

	entt::registry& Registry() {
		return m_sceneRegistry;
	}

	//void UpdateAllObjectsInScene(const float dt);
	entt::entity GetMainCamera() {
		return m_mainCamera;
	}
	//Camera& GetMainCamera(); 

	const std::string& SceneName() {
		return m_scene;
	}
	
	void UpdateTransforms();
	void ResetAllObjectsInScene();
	void CleanUpDestroyedObj();
private:
	friend class SceneManager;
	
	GameEntity& CreateBasicEntity(std::string name, bool isStatic = false, GameEntity::STATE state = GameEntity::STATE::ACTIVE);
	
	void SetMainCamera(entt::entity camera) {
		m_mainCamera = camera;
	}
	
	std::string m_scene;
	entt::entity m_mainCamera;
	entt::registry m_sceneRegistry;
};

