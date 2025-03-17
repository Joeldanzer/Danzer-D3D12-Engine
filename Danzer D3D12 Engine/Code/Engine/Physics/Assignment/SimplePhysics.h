#pragma once
#include "Core/MathDefinitions.h"

struct BoxCollider;
struct SphereCollider;

struct BaseCollider {
	BaseCollider(){}

	bool  m_gravity      = true;

	float m_mass         = 1.0f;
	float m_gravitySpeed = 9.81f;

	Vect3f m_center		 = {0.0f, 0.0f, 0.0f};
};

class SimplePhysics
{
public:
	SimplePhysics();
	~SimplePhysics();

	void SimulatePhysics();

private:
	bool BoxToSphereIntersect(const Entity boxEntity,  const Entity sphereEntity);
	bool BoxToBoxIntersect(const    Entity box1Entity, const Entity box2Entity);

	//bool SeparatingAxisTheorem
};

