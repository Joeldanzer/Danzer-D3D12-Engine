#pragma once
// The physics engine being used is JoltPhysics created by jrouwe @https://github.com/jrouwe/JoltPhysics
// Documentation: https://jrouwe.github.io/JoltPhysics/

#include <Jolt/Jolt.h> // Jolt main header, needs to be included first

#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

// RigidBodies and Shapes Includes
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <iostream>

using namespace JPH;

static constexpr float s_defaultGravity = -9.81f;

// Layer that objects can be in, determines which other objects it can collide with
namespace Layers
{
	static constexpr ObjectLayer UNUSED1	 = 0; // 4 unused values so that broadphase layers values don't match with object layer values
	static constexpr ObjectLayer UNUSED2	 = 1;
	static constexpr ObjectLayer UNUSED3	 = 2;
	static constexpr ObjectLayer UNUSED4	 = 3;
	static constexpr ObjectLayer NON_MOVING  = 4;
	static constexpr ObjectLayer MOVING		 = 5;
	static constexpr ObjectLayer DEBRIS		 = 6; // Example: Debris collides only with NON_MOVING
	static constexpr ObjectLayer SENSOR		 = 7; // Sensors only collide with MOVING objects
	static constexpr ObjectLayer NUM_LAYERS  = 8;
};

// Broadphase layers
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr BroadPhaseLayer DEBRIS(2);
	static constexpr BroadPhaseLayer SENSOR(3);
	static constexpr BroadPhaseLayer UNUSED(4);
	static constexpr uint32_t NUM_LAYERS(5);
};

// Function to override PhysicsJolt Tracing
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

#ifdef DEBUG
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine)
{
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

	// Breakpoint
	return true;
};
#endif