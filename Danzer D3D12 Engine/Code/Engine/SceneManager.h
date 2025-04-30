#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "entt/entt.hpp"

struct GameEntity;
struct Camera;

class SceneManager
{
public: 
	SceneManager();
	~SceneManager(){
	}

	entt::entity GetMainCamera() {
		return m_mainCamera;
	}
	void SetMainCamera(const entt::entity cameraEntity) {
		m_mainCamera = cameraEntity;
	}

	bool SetScene(std::string name, entt::entity camera, bool resetScene = false);


private:
	friend class Engine;

	void SearchForExistingScenes();

	void UpdateTransformsForRendering(bool updateStaticObjects = false);
	void UpdateLastPositions();

	//std::unordered_map<std::string, Scene> m_scenes;
	std::string m_currentScene;

	entt::entity m_mainCamera;

	// There will only be one Registry so when we switch scenes everything in that scene is removed
};

