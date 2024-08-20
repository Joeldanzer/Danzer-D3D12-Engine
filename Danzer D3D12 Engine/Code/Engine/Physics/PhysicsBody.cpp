#include "stdafx.h"
#include "PhysicsBody.h"

#include "PhysicsHandler.h"
#include "BodyInterfaceImpl.h"
#include "Components/GameEntity.h"

#include <Jolt/Physics/Body/BodyInterface.h>

PhysicsBody::PhysicsBody(const GameEntity& entity, BodyInterfaceImpl& bodyInterface, const JPH::BodyCreationSettings& settings, JPH::EActivation activation) :
	m_interface(&bodyInterface)
{
	m_id = m_interface->CreateAndAddBody(entity, settings, activation);
}

PhysicsBody::~PhysicsBody()
{
	//m_interface->RemoveBody(m_id);
	//m_interface->DestroyBody(m_id);
	m_interface = nullptr;	
}
 
void PhysicsBody::AddForce(Vect3f force)				   { m_interface->GetInterface()->AddForce(m_id, { force.x, force.y, force.z }); }
void PhysicsBody::AddLinearVelocity(Vect3f linearVelocity) { m_interface->GetInterface()->AddLinearVelocity(m_id, { linearVelocity.x, linearVelocity.y, linearVelocity.z }); }

void PhysicsBody::SetFriction(float friction)			   { m_interface->GetInterface()->SetFriction(m_id, friction); }
void PhysicsBody::SetGravityFactor(float gravityFactor)	   { m_interface->GetInterface()->SetGravityFactor(m_id, gravityFactor); }

void PhysicsBody::ActivateBody()
{
	if(!m_interface->GetInterface()->IsActive(m_id))
		m_interface->GetInterface()->ActivateBody(m_id);
}



