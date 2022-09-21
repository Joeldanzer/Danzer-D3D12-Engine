#pragma once
#include "../Engine/Core/MathDefinitions.h"

class Camera;
class Object;
class AABBCollider;

class CameraController 
{
public:
	CameraController() = delete;
	CameraController(Camera* cam, Vect3f offset);
	~CameraController();

	void SetCamera(Camera* cam);
	void SetTarget(Object& target) {
		m_target = &target;
	}
	void SetCameraBoundingBox(AABBCollider* boundigArea);
	void UpdateCamera(const float dt);
	
private:
	float m_smoothMovement;

	Vect3f m_outofBoundsPosition;

	Vect3f  m_offset;
	Camera* m_camera;
	Object* m_target;
	AABBCollider* m_boundingArea;

	Vect3f m_points[4];
};

