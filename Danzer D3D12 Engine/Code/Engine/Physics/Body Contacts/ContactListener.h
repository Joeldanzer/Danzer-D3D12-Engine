#pragma once

#include <Jolt/Physics/Collision/ContactListener.h>

#include <iostream>

namespace JPH {
	class Body;
	class CollideShapeResult;
	class ContactManifold;
	class ContactSettings;
	class SubShapeIDPair;
}

class BodyInterfaceImpl;

class ContactListenerImpl : public ContactListener
{
public:
	ContactListenerImpl(entt::registry& reg, BodyInterfaceImpl& bodyInterface) :
		m_registry(reg),
		m_interface(&bodyInterface)
	{}

	// See: ContactListener
	virtual ValidateResult OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override;
	virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
	virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
	virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;
private:
	BodyInterfaceImpl* m_interface;
	entt::registry&    m_registry;
};