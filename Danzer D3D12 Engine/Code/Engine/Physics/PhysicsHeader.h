#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

static constexpr uint32_t s_MaxPhysicsJobs     = 2048; // Maximum amount of jobs to allow
static constexpr uint32_t s_MaxPhysicsBarriers = 8;    // Maximum amount of barriers to Allow

static constexpr float s_defaultGravity = -9.81f;

// Layer that objects can be in, determines which other objects it can collide with
namespace Layers
{
	static constexpr JPH::ObjectLayer UNUSED1 = 0; // 4 unused values so that broadphase layers values don't match with object layer values
	static constexpr JPH::ObjectLayer UNUSED2 = 1;
	static constexpr JPH::ObjectLayer UNUSED3 = 2;
	static constexpr JPH::ObjectLayer UNUSED4 = 3;
	static constexpr JPH::ObjectLayer NON_MOVING = 4;
	static constexpr JPH::ObjectLayer MOVING = 5;
	static constexpr JPH::ObjectLayer DEBRIS = 6; // Example: Debris collides only with NON_MOVING
	static constexpr JPH::ObjectLayer SENSOR = 7; // Sensors only collide with MOVING objects
	static constexpr JPH::ObjectLayer NUM_LAYERS = 8;
};

// Broadphase layers
namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::BroadPhaseLayer DEBRIS(2);
	static constexpr JPH::BroadPhaseLayer SENSOR(3);
	static constexpr JPH::BroadPhaseLayer UNUSED(4);
	static constexpr uint32_t NUM_LAYERS(5);
};
