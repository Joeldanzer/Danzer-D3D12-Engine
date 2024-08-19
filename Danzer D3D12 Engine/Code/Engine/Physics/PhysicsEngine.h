#pragma once
#include "PhysicsLayers.h"

#define PHYSICS_THREADED

namespace JPH {
	class JobSystem;
	class PhysicsSystem;
	class TempAllocator;
	class ContactListenerImpl;
	class BodyActivationListenerImpl;
	class DebugRenderer;
}

class PhysicsEngine
{
private:
	static constexpr uint32_t s_MaxPhysicsJobs     = 2048; // Maximum amount of jobs to allow
	static constexpr uint32_t s_MaxPhysicsBarriers = 8;    // Maximum amount of barriers to Allow

	const uint32_t m_numberOfSteps = 1;
	const uint32_t m_maxConcurrentJobs;

public:
	 PhysicsEngine(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints, const UINT maxConcurrentJobs);
	~PhysicsEngine();

	void Update(const float physicsDT, const int collisionSteps);
	
	JPH::PhysicsSystem* GetPhysicsSystem() {
		return m_physicsSystem;
	}

private:
	JPH::JobSystem*					  m_jobSystem			   = nullptr;
	JPH::JobSystem*					  m_jobSystemValidating	   = nullptr;

	JPH::TempAllocator*				  m_tempAllocator		   = nullptr;
	JPH::PhysicsSystem*				  m_physicsSystem		   = nullptr;
	JPH::ContactListenerImpl*		  m_contactListener		   = nullptr;;
	JPH::BodyActivationListenerImpl*  m_bodyActivationListener = nullptr;

	JPH::PhysicsSettings			  m_physicsSettings;
	
	LayerInterfaceImpl				  m_layerInterface;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl         m_layerPairFilter;

	bool m_drawDebug;
};

