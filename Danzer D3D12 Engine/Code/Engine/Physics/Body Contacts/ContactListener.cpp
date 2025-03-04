#include "stdafx.h"

#include "Physics/PhysicsBody.h"
#include "Components/GameEntity.h"

#include "ContactListener.h"

#include "Physics/PhysicsHeader.h"
#include "Physics/BodyInterfaceImpl.h"

ValidateResult ContactListenerImpl::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
	//std::cout << "Contact validate callback" << std::endl;
	// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void ContactListenerImpl::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	auto entity = m_interface->FetchEntityFromBodyID(inBody1.GetID().GetIndexAndSequenceNumber());
	PhysicsBody& body1      = Reg::Instance()->Get<PhysicsBody>(entity);

	entity = m_interface->FetchEntityFromBodyID(inBody2.GetID().GetIndexAndSequenceNumber());
	GameEntity& gameEntity2 = Reg::Instance()->Get<GameEntity>(entity);

	body1.m_onContactAddedList.emplace_back(&gameEntity2);
}

void ContactListenerImpl::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	//std::cout << "A contact was persisted" << std::endl;
}

void ContactListenerImpl::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	auto entity = m_interface->FetchEntityFromBodyID(inSubShapePair.GetBody1ID().GetIndexAndSequenceNumber());
	PhysicsBody& body1 = RegistryWrapper::Instance()->Get<PhysicsBody>(entity);
	
	entity = m_interface->FetchEntityFromBodyID(inSubShapePair.GetBody2ID().GetIndexAndSequenceNumber());
	GameEntity& gameEntity2 = Reg::Instance()->Get<GameEntity>(entity);

	body1.m_onContactRemovedList.emplace_back(&gameEntity2);
}
