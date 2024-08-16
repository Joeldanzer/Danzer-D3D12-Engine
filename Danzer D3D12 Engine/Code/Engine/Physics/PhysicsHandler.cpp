#include "stdafx.h"

#include "PhysicsHandler.h"
#include "PhysicsEngine.h"
#include "Physics/PhysicsBody.h"

#include "Components/GameEntity.h"
#include "Components/Transform.h"

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

PhysicsHandler::PhysicsHandler(PhysicsEngine& physicsEngine) :
	m_physicsSystem(physicsEngine.GetPhysicsSystem()),
	m_bodyInterface(m_physicsSystem->GetBodyInterface())
{
}

PhysicsHandler::~PhysicsHandler()
{
	m_physicsSystem = nullptr;
}

PhysicsBody PhysicsHandler::CreatePhyscisSphere(float radius, EMotionType motionType, EActivation activation, ObjectLayer layer)
{
	BodyCreationSettings sphereSettings(new SphereShape(0.0f), RVec3::sZero(), Quat::sZero(), motionType, layer);
	return PhysicsBody(m_bodyInterface, sphereSettings, activation);
}

PhysicsBody PhysicsHandler::CreatePhysicsBox(Vect3f size, EMotionType motionType, EActivation activation, ObjectLayer layer)
{
	BodyCreationSettings boxSettings(new BoxShape({ size.x, size.y, size.z }), RVec3::sZero(), Quat::sZero(), motionType, layer);
	return PhysicsBody(m_bodyInterface, boxSettings, activation);
}


void PhysicsHandler::SetPhysicsPositionAndRotation(entt::registry& reg)
{
	auto view = reg.view<PhysicsBody>();
	for (entt::entity entity : view) {
		Transform* transform = reg.try_get<Transform>(entity);
		GameEntity* gameEntt = reg.try_get<GameEntity>(entity);
		if (!gameEntt || !transform) {
			reg.destroy(entity);
#ifdef DEBUG
			assert(gameEntt, "Entity with PhysicsBody component is missing 'GameEntity' and/or 'Transform' component!");
#endif 
		}

		PhysicsBody& body = reg.get<PhysicsBody>(entity);	
		if (m_bodyInterface.IsActive(body.m_id)) {
			Vect3f pos = transform->m_position;
			Quat4f rot = transform->m_rotation;

			m_bodyInterface.SetPositionAndRotation(body.m_id, { pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w }, EActivation::Activate);
		}
	}
}

void PhysicsHandler::UpdatePhysicsEntities(entt::registry& reg)
{
	auto view = reg.view<PhysicsBody>();
	for (entt::entity entity : view) {
		Transform& transform = reg.get<Transform>(entity);

		PhysicsBody& body = reg.get<PhysicsBody>(entity);
		if (m_bodyInterface.IsActive(body.m_id)) {
			RVec3 pos = m_bodyInterface.GetPosition(body.m_id);
			Quat rot  = m_bodyInterface.GetRotation(body.m_id);

			transform.m_position = { pos.GetX(), pos.GetY(), pos.GetZ() };
			transform.m_rotation = { rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW() };
		}
	}
}

