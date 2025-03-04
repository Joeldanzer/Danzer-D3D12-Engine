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
	SceneManager(Camera& cam);
	~SceneManager(){
		//m_scenes.clear();
	}

	//Scene& CreateEmptyScene(std::string sceneName);

	//void AddScene(std::string name, const Scene& scene);
	//void LoadScene(std::string sceneName);

	//entt::registry& Registry() {
	//	return m_registry;
	//}

	entt::entity GetMainCamera() {
		return m_mainCamera;
	}

	//Scene& GetSpecificScene(std::string sceneName) { return m_scenes[sceneName]; }
	bool SetScene(std::string name, entt::entity camera, bool resetScene = false);

	//Scene& GetCurrentScene() { 
	//	return m_scenes[m_currentScene]; 
	//}	

	//GameEntity& CreateBasicEntity(std::string name, bool isStatic);

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

