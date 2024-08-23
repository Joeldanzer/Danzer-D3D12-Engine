#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "Rendering/Camera.h"

#include "Scene.h"

class SceneManager
{
public: 
	SceneManager() : 
		m_scenes()
	{}
	~SceneManager(){
		m_scenes.clear();
	}

	void Init(Camera& cam);
	//void CreateLoadedScene(LevelScene* loadedScene, std::string sceneName = "");
	Scene& CreateEmptyScene(std::string sceneName);

	void AddScene(std::string name, const Scene& scene);

	Scene& GetSpecificScene(std::string sceneName) { return m_scenes[sceneName]; }
	bool SetScene(std::string name, entt::entity camera, bool resetScene = false);

	Scene& GetCurrentScene() { 
		return m_scenes[m_currentScene]; 
	}	

	GameEntity& CreateBasicEntity(std::string name, bool isStatic, GameEntity::STATE state = GameEntity::STATE::ACTIVE);

private:
	std::unordered_map<std::string, Scene> m_scenes;
	std::string m_currentScene;

	entt::registry m_registry;
};

