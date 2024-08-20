#include "stdafx.h"

#include "PhysicsHandler.h"
#include "PhysicsEngine.h"
#include "Physics/PhysicsBody.h"

#include "Components/GameEntity.h"
#include "Components/Transform.h"

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

PhysicsHandler::PhysicsHandler(PhysicsEngine& physicsEngine) :
	//m_physicsSystem(physicsEngine.GetPhysicsSystem()),
	m_bodyInterface(physicsEngine.GetBodyInterface())
{
}

PhysicsHandler::~PhysicsHandler()
{
	
}

PhysicsBody PhysicsHandler::CreatePhyscisSphere(const GameEntity& gameEntity, float radius, EMotionType motionType, EActivation activation, ObjectLayer layer)
{
	BodyCreationSettings sphereSettings(new SphereShape(radius <= 0.0f ? 0.1f : radius), RVec3::sZero(), Quat::sIdentity(), motionType, layer);
	return PhysicsBody(gameEntity, m_bodyInterface, sphereSettings, activation);
}

PhysicsBody PhysicsHandler::CreatePhysicsBox(const GameEntity& gameEntity, Vect3f size, EMotionType motionType, EActivation activation, ObjectLayer layer)
{
	BodyCreationSettings boxSettings(new BoxShape({ size.x, size.y, size.z }), RVec3::sZero(), Quat::sIdentity(), motionType, layer);
	return PhysicsBody(gameEntity, m_bodyInterface, boxSettings, activation);
}


void PhysicsHandler::SetPhysicsPositionAndRotation(entt::registry& reg)
{
	auto view = reg.view<PhysicsBody>();
	for (entt::entity entity : view) {
		PhysicsBody& body = reg.get<PhysicsBody>(entity);	
		
		if (m_bodyInterface.GetInterface()->GetMotionType(body.m_id) == EMotionType::Static)
			continue;
		
		Transform* transform = reg.try_get<Transform>(entity);
		GameEntity* gameEntt = reg.try_get<GameEntity>(entity);
		if (gameEntt || transform) {

			if (m_bodyInterface.GetInterface()->IsActive(body.m_id)) {
				Vect3f pos = transform->m_position;
				Quat4f rot = transform->m_rotation;
				m_bodyInterface.GetInterface()->SetPositionAndRotation(body.m_id, Vec3(pos.x, pos.y, pos.z ), Quat(rot.x, rot.y, rot.z, rot.w), EActivation::Activate);
			}
		}
		else {
			m_bodyInterface.GetInterface()->RemoveBody(body.m_id);
			m_bodyInterface.GetInterface()->DestroyBody(body.m_id);
			reg.destroy(entity);
#ifdef DEBUG
			assert(gameEntt, "Entity with PhysicsBody component is missing 'GameEntity' and/or 'Transform' component!");
#endif 
		}
	}
}

// Only called once and thats at the end of the Initilize frame
void PhysicsHandler::UpdateStaticColliders(entt::registry& reg)
{
	auto view = reg.view<PhysicsBody>();
	for (entt::entity entity : view) {
		PhysicsBody& body = reg.get<PhysicsBody>(entity);

		if (m_bodyInterface.GetInterface()->GetMotionType(body.m_id) != EMotionType::Static)
			continue;

		Transform* transform = reg.try_get<Transform>(entity);
		GameEntity* gameEntt = reg.try_get<GameEntity>(entity);
		if (gameEntt || transform) {

			//if (m_bodyInterface.IsActive(body.m_id)) {
			Vect3f pos = transform->m_position;
			Quat4f rot = transform->m_rotation;

			m_bodyInterface.GetInterface()->SetPositionAndRotation(body.m_id, Vec3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w), EActivation::Activate);
			//}
		}
		else {
			m_bodyInterface.GetInterface()->RemoveBody(body.m_id);
			m_bodyInterface.GetInterface()->DestroyBody(body.m_id);
			reg.destroy(entity);
#ifdef DEBUG
			assert(gameEntt, "Entity with PhysicsBody component is missing 'GameEntity' and/or 'Transform' component!");
#endif 
		}
	}
}

void PhysicsHandler::UpdatePhysicsEntities(entt::registry& reg)
{
	auto view = reg.view<PhysicsBody>();
	for (entt::entity entity : view) {
		PhysicsBody& body = reg.get<PhysicsBody>(entity);
		if (m_bodyInterface.GetInterface()->GetMotionType(body.m_id) == EMotionType::Static)
			continue;
		
		Transform& transform = reg.get<Transform>(entity);

		if (m_bodyInterface.GetInterface()->IsActive(body.m_id)) {
			RVec3 pos = m_bodyInterface.GetInterface()->GetPosition(body.m_id);
			Quat rot  = m_bodyInterface.GetInterface()->GetRotation(body.m_id);

			transform.m_position = { pos.GetX(), pos.GetY(), pos.GetZ() };
			transform.m_rotation = { rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW() };
		}
	}
}

