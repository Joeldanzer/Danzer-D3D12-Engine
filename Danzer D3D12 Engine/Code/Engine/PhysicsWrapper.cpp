#include "stdafx.h"
#include "PhysicsWrapper.h"



PhysicsWrapper::PhysicsWrapper(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints) :
	m_layerInterface(static_cast<JPH::BroadPhaseLayer::Type>(PhysicsLayers::NUM_LAYERS)),
	m_objectVsBriadPhaseLayerFilter(m_layerInterface)
{
	m_physicsSystem = new JPH::PhysicsSystem();

	m_physicsSystem->Init(maxBodies, maxBodyMutexes, maxBodyPairs, maxContactConstraints,
		m_layerInterface, m_objectVsBriadPhaseLayerFilter, m_layerPairFitlerMask);
	m_physicsSystem->SetPhysicsSettings(m_physicsSettings);
}

PhysicsWrapper::~PhysicsWrapper()
{
}
