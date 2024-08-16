#include "stdafx.h"
#include "PhysicsBody.h"

#include "PhysicsHandler.h"
#include <Jolt/Physics/Body/BodyInterface.h>

PhysicsBody::PhysicsBody(BodyInterface& bodyInterface, const BodyCreationSettings& settings, EActivation activation) :
	m_interface(&bodyInterface)
{
	m_id = m_interface->CreateAndAddBody(settings, activation);
}

PhysicsBody::~PhysicsBody()
{
	m_interface->RemoveBody(m_id);
}

void PhysicsBody::AddForce(Vect3f force)				   { m_interface->AddForce(m_id, { force.x, force.y, force.z }); }
void PhysicsBody::AddLinearVelocity(Vect3f linearVelocity) { m_interface->AddLinearVelocity(m_id, { linearVelocity.x, linearVelocity.y, linearVelocity.z }); }

void PhysicsBody::SetFriction(float friction)			   { m_interface->SetFriction(m_id, friction); }
void PhysicsBody::SetGravityFactor(float gravityFactor)	   { m_interface->SetGravityFactor(m_id, gravityFactor); }

//void PhysicsBody::SetBodySettings(BodyCreationSettings settings)
//{
//	
//}


