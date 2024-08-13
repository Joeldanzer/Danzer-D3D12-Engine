#pragma once

#include "Core/D3D12Header.h"

#include "PhysicsLayers.h"
#include <Jolt/Physics/PhysicsSettings.h>

#include <thread>
#include <iostream>

namespace JPH {
	class JobSystem;
	class PhysicsSystem;
}

static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	std::cout << buffer << std::endl;
}

static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine)
{
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

	// Breakpoint
	return true;
};

class PhysicsWrapper
{
public:
	 PhysicsWrapper(const UINT maxBodies, const UINT maxBodyMutexes, const UINT maxBodyPairs, const UINT maxContactConstraints);
	~PhysicsWrapper();

private:

	JPH::JobSystem* m_jobSystem = nullptr;
	JPH::JobSystem* m_jobSystemValidating = nullptr;
	
	JPH::PhysicsSystem* m_physicsSystem = nullptr;
	
	uint32_t m_maxConcurrentJobs = std::thread::hardware_concurrency();
	
	LayerInterfaceImpl				  m_layerInterface;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl         m_layerPairFilter;

	JPH::PhysicsSettings			  m_physicsSettings;
};

