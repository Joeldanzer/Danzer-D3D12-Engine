#pragma once

#include "entt/entt.hpp"

class Engine;

struct DirectionalLight;
struct Model;


class EditorLoadAndSave
{
public:
	EditorLoadAndSave(Engine& engine);
	~EditorLoadAndSave();

private:
	Engine& m_engine;
};

