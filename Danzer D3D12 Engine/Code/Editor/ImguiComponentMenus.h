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
	static void DisplayComponentData(Model& model);

	// Lighting 
	static void DisplayComponentData(PointLight& pointLight);
	static void DisplayComponentData(DirectionalLight& dirLight);
	static void DisplayComponentData(Camera& canera);

	static void DisplayComponentSelection(entt::registry& reg, entt::entity entity);

};