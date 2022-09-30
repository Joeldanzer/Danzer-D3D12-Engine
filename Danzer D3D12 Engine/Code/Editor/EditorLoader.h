#pragma once
#include <string>
#include <vector>

#include "../3rdParty/entt-master/single_include/entt/entt.hpp"

class Engine;

class EditorLoader
{
public:
	EditorLoader(Engine& engine);
	~EditorLoader();

	void LoadScene(entt::registry& reg, std::string scene);
	void SaveScene(entt::registry& reg);

private:
	void LoadComponentsAndValues(entt::entity entity);

	Engine& m_engine;
	std::string m_currentScene;
};

