#pragma once
#include "ImguiHandler.h"

class Engine;

class Editor
{
public:
	Editor(Engine& engine);
	~Editor();

	void Update(const float deltaTime);

private:
	void CameraControlls(const float dt);

	Engine& m_engine;
	//entt::entity m_camera;

	float m_turnSpeed;
	float m_moveSpeed;


	ImguiHandler m_imguiHandler;
};

