#include "stdafx.h"
#include "PhysicsWrapper.h"

#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/JobSystemSingleThreaded.h"

PhysicsWrapper::PhysicsWrapper(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints) :
	m_layerInterface(),
	m_layerPairFilter(),
	m_objectVsBroadPhaseLayerFilter()
{
	//m_jobSystem			  = new JPH::JobSystemThreadPool(2048, 8, 1);
	//m_jobSystemValidating = new JPH::JobSystemSingleThreaded(2048);
	//
	//m_physicsSystem = new JPH::PhysicsSystem();
	//m_physicsSystem->Init(maxBodies, maxBodyMutexes, maxBodyPairs, maxContactConstraints,
	//				      m_layerInterface, m_objectVsBroadPhaseLayerFilter, m_layerPairFilter);
	//m_physicsSystem->SetPhysicsSettings(m_physicsSettings);
}

PhysicsWrapper::~PhysicsWrapper()
{
}
