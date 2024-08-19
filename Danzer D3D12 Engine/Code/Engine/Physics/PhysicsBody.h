#pragma once

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace JPH {
	class BodyInterface;
	class BodyCreationSettings;
}

struct PhysicsBody {
	PhysicsBody() = delete;
	explicit PhysicsBody(JPH::BodyInterface& bodyInterface, const JPH::BodyCreationSettings& settings, JPH::EActivation activation);
	//explicit PhysicsBody(const PhysicsBody&) = default;
	~PhysicsBody();

	void AddForce(Vect3f force);
	void AddLinearVelocity(Vect3f linearVelocity);
	
	void SetFriction(float friction);
	void SetGravityFactor(float gravityFactor);

	// This is set on PhysicsBody creation in PhysicsHandler, only used if entity is changing the shape.
	//void SetBodySettings(BodyCreationSettings settings);

private:
	friend class PhysicsHandler;
	
	JPH::BodyID m_id;
	JPH::BodyInterface* m_interface = nullptr;
};


