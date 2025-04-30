#pragma once
#include "ImguiHandler.h"
#include "entt/entt.hpp"

class Camera;
class Transform;

class Editor
{
public:
	static Editor& Instance() {
		if (!s_instance)
			s_instance = new Editor();

		return *s_instance;
	}

	void Update(const float deltaTime);

	Transform& EditorCamTransform() {
		return *m_editorCamTransform;
	}
	Camera& EditorCam() {
		return *m_editorCam;
	}
private:
	Editor();
	~Editor();
	
	Camera*	   m_editorCam;
	Transform* m_editorCamTransform;

	void CameraControlls(const float dt);

	float m_turnSpeed;
	float m_moveSpeed;
	
	ImguiHandler m_imguiHandler;

	static Editor* s_instance;
};

