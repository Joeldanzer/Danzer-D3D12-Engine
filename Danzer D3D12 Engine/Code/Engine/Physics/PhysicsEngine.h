#pragma once
#include "PhysicsLayers.h"
#include "BodyInterfaceImpl.h"

#define PHYSICS_THREADED

namespace JPH {
	class JobSystem;
	class PhysicsSystem;
	class TempAllocator;
	class BodyActivationListenerImpl;
	class DebugRenderer;
	class ContactListener;
}


class PhysicsEngine
{
private:
	static constexpr uint32_t s_MaxPhysicsJobs     = 2048; // Maximum amount of jobs to allow
	static constexpr uint32_t s_MaxPhysicsBarriers = 8;    // Maximum amount of barriers to Allow
	 
	const uint32_t m_numberOfSteps = 1;
	const uint32_t m_maxConcurrentJobs;

public:
	 PhysicsEngine(
		 const UINT maxBodies, 
		 const UINT maxBodyMutexes, 
		 const UINT maxBodyPairs, 
		 const UINT maxContactConstraints, 
		 const UINT maxConcurrentJobs
	 );
	~PhysicsEngine();

	void Update(const float physicsDT, const int collisionSteps);
	void OptimizeBroadPhase();

	JPH::PhysicsSystem* GetPhysicsSystem() {
		return m_physicsSystem;
	}
	
	BodyInterfaceImpl& GetBodyInterface() {
		return m_bodyInterface;
	}

private:
	JPH::JobSystem*					  m_jobSystem			   = nullptr;
	JPH::JobSystem*					  m_jobSystemValidating	   = nullptr;

	JPH::TempAllocator*				  m_tempAllocator		   = nullptr;
	JPH::PhysicsSystem*				  m_physicsSystem		   = nullptr;
	JPH::BodyActivationListenerImpl*  m_bodyActivationListener = nullptr;
	JPH::ContactListener*		      m_contactListener		   = nullptr;;

	JPH::PhysicsSettings			  m_physicsSettings;
	
	BodyInterfaceImpl				  m_bodyInterface;
	LayerInterfaceImpl				  m_layerInterface;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl         m_layerPairFilter;

	bool m_drawDebug = false;
};

