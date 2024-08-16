#pragma once

#include "entt/entt.hpp"
#include "PhysicsBody.h"

#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include <Core/MathDefinitions.h>

class PhysicsEngine;

namespace JPH {
	class PhysicsSystem;
	class BodyInterface;
}

class PhysicsHandler
{
public:
	PhysicsHandler(PhysicsEngine& physicsEngine);
	~PhysicsHandler();

	PhysicsBody CreatePhyscisSphere(float radius, JPH::EMotionType motionType, JPH::EActivation activation, JPH::ObjectLayer layer);
	PhysicsBody CreatePhysicsBox   (Vect3f size, JPH::EMotionType motionType, JPH::EActivation activation, ObjectLayer layer);

private:
	// Sets Physics Position and Rotation before PhysicsSystem.Update()
	void SetPhysicsPositionAndRotation(entt::registry& reg);
	// Update all the transform of entites after PhysicsSystem.Update() so the that the results are repclicated in rendering
	void UpdatePhysicsEntities(entt::registry& reg);

	JPH::PhysicsSystem* m_physicsSystem = nullptr;
	JPH::BodyInterface& m_bodyInterface;

	// Used to optimize simulation when many bodies have been created when for example,
	// loading a new level.
	bool optimizeBroadPhase = false; 
	
	friend class Engine;
};

