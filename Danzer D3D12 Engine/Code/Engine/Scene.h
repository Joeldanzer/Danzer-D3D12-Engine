#pragma once
#include "entt/entt.hpp"
#include "Components/Object.h"

class Scene
{
public:
	Scene();
	Scene(const Scene& scene) {}
	~Scene();

	entt::registry& Registry() {
		return m_sceneRegistry;
	}
	entt::entity CreateBasicEntity(
		std::string name, std::string tag = "default", unsigned int layer = 0, bool isStatic = false, Object::STATE state = Object::STATE::ACTIVE);

	//void UpdateAllObjectsInScene(const float dt);
	entt::entity GetMainCamera() {
		return m_mainCamera;
	}
	//Camera& GetMainCamera(); 

	
	void UpdateTransforms();
	void ResetAllObjectsInScene();
private:
	friend class SceneManager;
	
	void SetMainCamera(entt::entity camera) {
		m_mainCamera = camera;
	}
	
	std::string m_scene;
	entt::entity m_mainCamera;
	entt::registry m_sceneRegistry;
};

