#pragma once

#include "Components/ComponentRegister.h"

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace JPH {
	class BodyCreationSettings;
}

struct GameEntity;
class BodyInterfaceImpl;

struct PhysicsBody : public BaseComponent {
	COMP_FUNC(PhysicsBody)
	PhysicsBody(){}
	explicit PhysicsBody(const GameEntity& entity, BodyInterfaceImpl& bodyInterface, const JPH::BodyCreationSettings& settings, JPH::EActivation activation);
	~PhysicsBody();

	void AddForce(const Vect3f force);
	void AddLinearVelocity(const Vect3f linearVelocity);
	
	void SetFriction(const float friction);
	void SetGravityFactor(const float gravityFactor);

	//void SetOnContactAdded(void (*contactAdded)(GameEntity& collidedEntity)) {
	//	OnContactAdded = contactAdded;
	//}

	// void* function for this PhysicsComponent, override it with a function for it to be called.
	std::function<void(GameEntity&)> OnContactRemoved = nullptr;
	std::function<void(GameEntity&)> OnContactAdded   = nullptr; 
	
	//void (*OnContactAdded)(GameEntity&) = nullptr;

	void ActivateBody();

private:
	friend class PhysicsHandler;
	friend class ContactListenerImpl;
	
	std::vector<GameEntity*> m_onContactRemovedList;
	std::vector<GameEntity*> m_onContactAddedList;

	JPH::BodyID m_id;
	BodyInterfaceImpl* m_interface = nullptr;
};
REGISTER_COMPONENT(PhysicsBody)


