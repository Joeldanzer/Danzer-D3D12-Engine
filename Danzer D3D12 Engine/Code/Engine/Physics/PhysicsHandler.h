#pragma once

#include "entt/entt.hpp"
#include "PhysicsBody.h"

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include <Core/MathDefinitions.h>

class BodyInterfaceImpl;
class PhysicsEngine;
struct GameEntity;

class PhysicsHandler
{
public:
	PhysicsHandler(PhysicsEngine& physicsEngine);
	~PhysicsHandler();

	PhysicsBody CreatePhyscisSphere(const GameEntity& gameEntity, float radius, JPH::EMotionType motionType, JPH::EActivation activation, JPH::ObjectLayer layer);
	PhysicsBody CreatePhysicsBox   (const GameEntity& gameEntity, Vect3f size,  JPH::EMotionType motionType, JPH::EActivation activation, JPH::ObjectLayer layer);

private:
	// Sets Physics Position and Rotation before PhysicsSystem.Update()
	void SetPhysicsPositionAndRotation();
	void UpdateStaticColliders();

	// Update all the transform of entites after PhysicsSystem.Update() so the that the results are repclicated in rendering
	void UpdatePhysicsEntities();

	BodyInterfaceImpl&  m_bodyInterface;

	// Used to optimize simulation when many bodies have been created when for example,
	// loading a new level.
	bool optimizeBroadPhase = false; 
	

	friend class Engine;
};

