#pragma once

#include "entt/entt.hpp"
#include <Jolt/Physics/Body/BodyID.h>

class PhysicsEngine;

namespace JPH {
	class PhysicsSystem;
	class BodyInterface;
}

class PhysicsHandler
{
public:
	PhysicsHandler(PhysicsEngine& physicsEngine);
	
	// Physics body is a pure virtual class
	struct PhysicsBody {	
		PhysicsBody(JPH::BodyInterface& bodyInterface) :
			m_interface(bodyInterface)
		{}
	protected:
		friend class PhysicsHandler;
		JPH::BodyID m_id;
		JPH::BodyInterface& m_interface;
	};

private:
	void UpdatePhysicsEntities(entt::registry& reg);

	JPH::PhysicsSystem* m_physicsSystem = nullptr;
	JPH::BodyInterface& m_bodyInterface;

	// Used to optimize simulation when many bodies have been created when for example,
	// loading a new level.
	bool optimizeBroadPhase = false; 
	
	friend class Engine;
};

