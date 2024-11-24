#include "stdafx.h"

#include "PhysicsEngine.h"
#include "PhysicsHandler.h"

#include "Body Contacts/ContactListener.h"
#include "Body Contacts/BodyActivationListener.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>

#include <thread>

using namespace JPH;

PhysicsEngine::PhysicsEngine(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints, const UINT maxConcurrentJobs) :
	m_layerInterface(),
	m_layerPairFilter(),
	m_objectVsBroadPhaseLayerFilter(),
	m_maxConcurrentJobs(maxConcurrentJobs > std::thread::hardware_concurrency() - 1 ? std::thread::hardware_concurrency() - 1 : maxConcurrentJobs)

{
	RegisterDefaultAllocator();
	
	Trace = TraceImpl;
#ifdef DEBUG
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl);
#endif

	Factory::sInstance = new JPH::Factory();
	bool verify = JPH::VerifyJoltVersionID(); // Verify that Jolt is compatible
	if (!verify)
		throw verify;

	RegisterTypes();

	// Temporarly allocate memory for physics update to avoid having to allocate memory during runtime
	m_tempAllocator		  = new TempAllocatorImpl(32 * 1024 * 1024);

#ifdef PHYSICS_THREADED
	//m_jobSystem			  = new JobSystemThreadPool(s_MaxPhysicsJobs, s_MaxPhysicsBarriers, m_maxConcurrentJobs);
	m_jobSystemValidating = new JobSystemSingleThreaded(s_MaxPhysicsJobs);
#else
	m_jobSystem = new JobSystemSingleThreaded(s_MaxPhysicsJobs);
#endif

	m_physicsSystem = new PhysicsSystem();
	m_physicsSystem->Init(maxBodies, maxBodyMutexes, maxBodyPairs, maxContactConstraints, m_layerInterface, m_objectVsBroadPhaseLayerFilter, m_layerPairFilter);
	
	m_bodyInterface.SetBodyInterface(m_physicsSystem->GetBodyInterface());

	m_bodyActivationListener = new BodyActivationListenerImpl;
	m_physicsSystem->SetBodyActivationListener(m_bodyActivationListener);

	Vec3 gravity(0.0f, s_defaultGravity, 0.0f);
	m_physicsSystem->SetGravity(gravity);

	m_physicsSystem->SetPhysicsSettings(m_physicsSettings);
}

PhysicsEngine::~PhysicsEngine()
{
	delete m_physicsSystem;
	delete m_contactListener;
	delete m_bodyActivationListener;
	delete m_tempAllocator;
	delete m_jobSystem;
	if (m_jobSystemValidating) delete m_jobSystemValidating; // Only delete if we are doing physics on multiple threads
}

void PhysicsEngine::Update(const float physicsDT, const int collisionSteps)
{
	//m_physicsSystem->Update(physicsDT, m_numberOfSteps, m_tempAllocator, m_jobSystem);
}

void PhysicsEngine::OptimizeBroadPhase()
{
	m_physicsSystem->OptimizeBroadPhase();
}

void PhysicsEngine::SetRegistry(entt::registry& registry)
{
	m_contactListener = new ContactListenerImpl(registry, m_bodyInterface);
	m_physicsSystem->SetContactListener(m_contactListener);
}
