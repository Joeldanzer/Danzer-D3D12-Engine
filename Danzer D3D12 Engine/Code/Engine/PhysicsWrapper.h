#pragma once
#include "Core/D3D12Header.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h"
#include "Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h"
#include "Jolt/Physics/Collision/ObjectLayerPairFilterMask.h"


enum PhysicsLayers : JPH::ObjectLayer {
	UNUSED1	   = 0,
	UNUSED2	   = 1,
	UNUSED3	   = 2,
	UNUSED4	   = 3,
	MOVING     = 4,
	NON_MOVING = 5,
	NUM_LAYERS = 6	
};

class PhysicsWrapper
{
public:
	 PhysicsWrapper(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints);
	~PhysicsWrapper();

private:
	JPH::PhysicsSystem*					   m_physicsSystem;
	JPH::BroadPhaseLayerInterfaceMask	   m_layerInterface;
	JPH::ObjectVsBroadPhaseLayerFilterMask m_objectVsBriadPhaseLayerFilter;
	JPH::ObjectLayerPairFilterMask         m_layerPairFitlerMask;
	JPH::PhysicsSettings				   m_physicsSettings;
};

