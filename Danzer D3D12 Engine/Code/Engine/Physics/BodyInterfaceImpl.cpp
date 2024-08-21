#include "stdafx.h"
#include "BodyInterfaceImpl.h"

#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsBody.h"

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "Components/GameEntity.h"
//BodyInterfaceImpl::BodyInterfaceImpl(PhysicsEngine& physicsEngine)
//{
//}

//const uint32 BodyInterfaceImpl::CreateAndAddBody(const JPH::BodyCreationSettings&, JPH::EActivation activation)
//{
//	//uint32 enttID = BodyID::cInvalidEnttID;
//	//m_physicsBodies.emplace_back(m_interface.add)
//
//}


BodyInterfaceImpl::BodyInterfaceImpl()
{
}

BodyID BodyInterfaceImpl::CreateAndAddBody(const GameEntity& entity, const JPH::BodyCreationSettings& bodySettings, JPH::EActivation activation)
{
	BodyID id = m_interface->CreateAndAddBody(bodySettings, activation);
	m_physicsBodies.insert({id.GetIndexAndSequenceNumber(), entity.m_entity });
	return id;
}

entt::entity BodyInterfaceImpl::FetchEntityFromBodyID(uint32 bodyID)
{
	return m_physicsBodies[bodyID];
}
