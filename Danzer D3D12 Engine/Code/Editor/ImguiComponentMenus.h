#pragma once

#include "entt/entt.hpp"

struct PointLight;
struct SpotLight;
struct GameEntity;
struct Model;
struct DirectionalLight;

class  Transform;
class  Camera;

class ModelHandler;


// Really want to find a nicer way to do this...
class ImguiComponentMenus {
public:
	// General 
	static void DisplayComponentData(GameEntity& gameEntity);
	static void DisplayComponentData(Transform& transform);

	static void DisplayComponentSelection(entt::entity entity);
};