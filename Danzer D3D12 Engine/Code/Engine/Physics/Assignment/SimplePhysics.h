#pragma once
#include "Components/Transform.h"
#include "Core/MathDefinitions.h"
#include "Core/RegistryWrapper.h"

struct BoxCollider;
struct SphereCollider;
struct ARay;

struct BaseCollider {
	BaseCollider(){}

	bool  m_drawCollider = false; // Meant for debug
	bool  m_gravity      = true;
	bool  m_kinematic    = false;

	float m_mass         = 1.0f;

	Vect3f m_center		 = {0.0f, 0.0f, 0.0f};
	Vect3f m_velocity    = { 0.0f, 0.0f, 0.0f };

	// std::function for when Entity collides with another Entity or it's contact is removed
	std::function<void(const Entity entity)> m_OnContactAdded = nullptr;

private:
	friend class SimplePhysics;

};

class SimplePhysics
{
public:
	SimplePhysics(){}
	~SimplePhysics(){}

	void SimulatePhysics(const float dt);
	
private:
	struct Collision {
		Collision() = delete;
		//Collision(BaseCollider*)
		//std::string m_colliderOne;
		//std::string m_colliderTwo;

		Vect3f m_collisionPoint;
	};
	
	bool m_startPhysics = false;

	const float m_gravityMultiplier = 9.81f;
	
	void ApplyGravity(const float dt);
	void ApplyVelocity(const float dt);

	void HandleCollision();
	void CheckRayCasts();

	template<typename T>
	void UpdateTransforms();

	void RayToBoxIntersect(const ARay& ray,    const Entity box);
	const bool RayEntryForBox(const ARay&, const BoxCollider& box);
	
	void RayToSphereIntersect(const ARay& ray, const Entity sphere);

	const bool BoxToSphereIntersect(const Entity boxEntity,  const Entity sphereEntity);
	const bool BoxToBoxIntersect(const Entity box1Entity, const Entity box2Entity);
	const bool SphereToShereIntersect(const Entity sphere1, const Entity sphere2);

	bool SeparatingAxisTheoremForBox(const Vect3f& translation, const Mat4f& absMatrix, const Vect3f& extent1, const Vect3f& extent2);
};

template<typename T>
inline void SimplePhysics::UpdateTransforms()
{

}
