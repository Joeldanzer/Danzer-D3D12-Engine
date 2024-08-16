#include "stdafx.h"

#include "PhysicsHandler.h"
#include "PhysicsHeader.h"
#include "PhysicsEngine.h"

#include <Jolt/Physics/PhysicsSystem.h>

using namespace JPH;

PhysicsHandler::PhysicsHandler(PhysicsEngine& physicsEngine) :
	m_physicsSystem(physicsEngine.GetPhysicsSystem()),
	m_bodyInterface(m_physicsSystem->GetBodyInterface())
{	
	//BodyID
}

void PhysicsHandler::UpdatePhysicsEntities(entt::registry& reg)
{
}
