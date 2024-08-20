#pragma once

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace JPH {
	class BodyCreationSettings;
}

struct GameEntity;
class BodyInterfaceImpl;

struct PhysicsBody {
	PhysicsBody() = delete;
	explicit PhysicsBody(const GameEntity& entity, BodyInterfaceImpl& bodyInterface, const JPH::BodyCreationSettings& settings, JPH::EActivation activation);
	//explicit PhysicsBody(const PhysicsBody&) = default;
	~PhysicsBody();

	void AddForce(const Vect3f force);
	void AddLinearVelocity(const Vect3f linearVelocity);
	
	void SetFriction(const float friction);
	void SetGravityFactor(const float gravityFactor);

	//void SetOnContactAdded(void (*contactAdded)(GameEntity& collidedEntity)) {
	//	OnContactAdded = contactAdded;
	//}

	std::function<void(GameEntity&)> OnContactAdded = nullptr;

	//void (*OnContactAdded)(GameEntity&) = nullptr;

	void ActivateBody();

private:
	friend class PhysicsHandler;

	void (*OnConcactRemoved)(GameEntity& collidedEntity) = nullptr;
	

	JPH::BodyID m_id;
	BodyInterfaceImpl* m_interface = nullptr;
};


