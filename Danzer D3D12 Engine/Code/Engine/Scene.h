#pragma once
#include "../3rdParty/entt/entt.hpp"
#include "Components/GameEntity.h"

class Scene
{
public:
	Scene();
	Scene(std::string sceneName, std::string fileLocation = "Scenes/");
	Scene(std::string path, entt::registry& reg);
	Scene(const Scene& scene) {}
	~Scene();

	const std::string& SceneName() {
		return m_name;
	}

private:
	friend class SceneManager;

	void LoadEntityAndComponentsToScene(entt::registry& reg);

	std::string m_filePath;
	std::string m_name;
};

