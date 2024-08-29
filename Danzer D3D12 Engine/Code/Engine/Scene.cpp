#include "stdafx.h"
#include "Scene.h"

#include "Components/GameEntity.h"
#include "Components/Transform.h"

#include <string>
#include <fstream>

Scene::Scene() : m_filePath(""), m_name("") {}
Scene::Scene(std::string sceneName, std::string fileLocation)
{
	std::ofstream outFile(sceneName + ".sce");
	outFile.close();
}
Scene::Scene(std::string path, entt::registry& reg) :
	m_filePath(path)
{
	LoadEntityAndComponentsToScene(reg);
}
Scene::~Scene(){}

void Scene::LoadEntityAndComponentsToScene(entt::registry& reg)
{
	// Add later
}
