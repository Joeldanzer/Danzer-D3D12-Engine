#include "EditorLoader.h"

EditorLoader::EditorLoader(Engine& engine) :
	m_engine(engine),
	m_currentScene("")
{
}

EditorLoader::~EditorLoader()
{
}

void EditorLoader::LoadScene(entt::registry& reg, std::string scene)
{
}

void EditorLoader::SaveScene(entt::registry& reg)
{
}
