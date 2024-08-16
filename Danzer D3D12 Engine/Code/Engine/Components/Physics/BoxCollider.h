#pragma once
#include "Physics/PhysicsBody.h"
#include "Core/MathDefinitions.h"

struct BoxCollider : public PhysicsBody {
	BoxCollider() = delete;
	explicit BoxCollider(JPH::BodyInterface& bodyInterface);

private:
};