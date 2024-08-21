#pragma once

#include <map>
#include <Jolt/Physics/Body/BodyID.h>

struct PhysicsBody;
class PhysicsEngine;

namespace JPH {
	class BodyInterface;
}

class BodyInterfaceImpl
{
public:
	BodyInterfaceImpl();

	JPH::BodyID CreateAndAddBody(const GameEntity& entity, const JPH::BodyCreationSettings& bodySettings, JPH::EActivation activation);
	JPH::BodyInterface* GetInterface() {
		return m_interface;
	}

	entt::entity FetchEntityFromBodyID(uint32 bodyID);

private:
	friend class PhysicsEngine;
	void SetBodyInterface(JPH::BodyInterface& bodyInterface){m_interface = &bodyInterface;}

	JPH::BodyInterface* m_interface = nullptr;
	std::unordered_map<uint32, entt::entity> m_physicsBodies = {};
};

