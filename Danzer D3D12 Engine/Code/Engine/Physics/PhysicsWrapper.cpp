#include "stdafx.h"
#include "PhysicsWrapper.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>

using namespace JPH;

PhysicsWrapper::PhysicsWrapper(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints) :
	m_layerInterface(),
	m_layerPairFilter(),
	m_objectVsBroadPhaseLayerFilter()
{
	RegisterDefaultAllocator();
	
	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl);

	Factory::sInstance = new JPH::Factory();

	bool verify = JPH::VerifyJoltVersionID(); // Verify that Jolt is compatible
	if (!verify)
		throw verify;

	RegisterTypes();
	TempAllocatorImpl tempAllocator(32 * 1024 * 1024);

	m_jobSystem			  = new JobSystemThreadPool(s_MaxPhysicsJobs, s_MaxPhysicsBarriers, m_maxConcurrentJobs);
	m_jobSystemValidating = new JobSystemSingleThreaded(s_MaxPhysicsJobs);

	m_physicsSystem = new PhysicsSystem();
	m_physicsSystem->Init(maxBodies, maxBodyMutexes, maxBodyPairs, maxContactConstraints, m_layerInterface, m_objectVsBroadPhaseLayerFilter, m_layerPairFilter);
	m_physicsSystem->SetPhysicsSettings(m_physicsSettings);
	
	Vec3 gravity(0.0f, s_defaultGravity, 0.0f);
	m_physicsSystem->SetGravity(gravity);
}

PhysicsWrapper::~PhysicsWrapper()
{
}
