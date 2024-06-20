#pragma once

#include "Core/D3D12Header.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "PhysicsLayers.h"


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
	JPH::JobSystem* m_jobSystem = nullptr;
	JPH::JobSystem* m_jobSystemValidating = nullptr;

	JPH::PhysicsSystem*			      m_physicsSystem = nullptr;
	LayerInterfaceImpl				  m_layerInterface;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl         m_layerPairFilter;
	JPH::PhysicsSettings			  m_physicsSettings;
};

