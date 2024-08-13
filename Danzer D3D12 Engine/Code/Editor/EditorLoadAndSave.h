#pragma once

#include "entt/entt.hpp"

#include "rapidjson/include/rapidjson/stringbuffer.h"
#include "rapidjson/include/rapidjson/prettywriter.h"

#include "rapidjson/include/rapidjson/document.h"

class Engine;

struct DirectionalLight;
struct Model;


class EditorLoadAndSave
{
public:
	EditorLoadAndSave(Engine& engine);
	~EditorLoadAndSave();

	bool LoadScene(std::string scene, entt::registry& reg);
	void SaveScene(std::string scene, entt::registry& reg); 

	std::string& CurrentScene() {
		return m_currentScene;
	}

private:

	void SaveModelSettings(Model* model, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void GetModelSettings(entt::entity, entt::registry& reg, rapidjson::Value& value);

	void SaveDirectionalLight(DirectionalLight* dirLight, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void GetDirectionalLightSettings(entt::entity, entt::registry& reg, rapidjson::Value& value);

	//void SaveCamera(Camera* camera, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	//void GetCameraSettings(entt::entity entity, entt::registry& reg, rapidjson::Value& value);

	//bool SceneExists(std::string nameOfScene);
	bool ReadJsonDocument(const std::string& scene, rapidjson::Document& doc);

	std::string m_currentScene;

	Engine& m_engine;
};

