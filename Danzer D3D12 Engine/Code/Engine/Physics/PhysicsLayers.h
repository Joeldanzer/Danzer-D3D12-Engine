#pragma once
#include "Core/D3D12Header.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"

/// Layer that objects can be in, determines which other objects it can collide with
namespace Layers
{
	static constexpr JPH::ObjectLayer UNUSED1 = 0; // 4 unused values so that broadphase layers values don't match with object layer values (for testing purposes)
	static constexpr JPH::ObjectLayer UNUSED2 = 1;
	static constexpr JPH::ObjectLayer UNUSED3 = 2;
	static constexpr JPH::ObjectLayer UNUSED4 = 3;
	static constexpr JPH::ObjectLayer NON_MOVING = 4;
	static constexpr JPH::ObjectLayer MOVING = 5;
	static constexpr JPH::ObjectLayer DEBRIS = 6; // Example: Debris collides only with NON_MOVING
	static constexpr JPH::ObjectLayer SENSOR = 7; // Sensors only collide with MOVING objects
	static constexpr JPH::ObjectLayer NUM_LAYERS = 8;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::UNUSED1:
		case Layers::UNUSED2:
		case Layers::UNUSED3:
		case Layers::UNUSED4:
			return false;
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING || inObject2 == Layers::DEBRIS;
		case Layers::MOVING:
			return inObject2 == Layers::NON_MOVING || inObject2 == Layers::MOVING || inObject2 == Layers::SENSOR;
		case Layers::DEBRIS:
			return inObject2 == Layers::NON_MOVING;
		case Layers::SENSOR:
			return inObject2 == Layers::MOVING;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

/// Broadphase layers
namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::BroadPhaseLayer DEBRIS(2);
	static constexpr JPH::BroadPhaseLayer SENSOR(3);
	static constexpr JPH::BroadPhaseLayer UNUSED(4);
	static constexpr UINT NUM_LAYERS(5);
};

class LayerInterfaceImpl : public JPH::BroadPhaseLayerInterface
{
public:
	LayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		m_ObjectToBroadPhase[Layers::UNUSED1] = BroadPhaseLayers::UNUSED;
		m_ObjectToBroadPhase[Layers::UNUSED2] = BroadPhaseLayers::UNUSED;
		m_ObjectToBroadPhase[Layers::UNUSED3] = BroadPhaseLayers::UNUSED;
		m_ObjectToBroadPhase[Layers::UNUSED4] = BroadPhaseLayers::UNUSED;
		m_ObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		m_ObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		m_ObjectToBroadPhase[Layers::DEBRIS] = BroadPhaseLayers::DEBRIS;
		m_ObjectToBroadPhase[Layers::SENSOR] = BroadPhaseLayers::SENSOR;
	}

	virtual UINT GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return m_ObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::DEBRIS:		return "DEBRIS";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::SENSOR:		return "SENSOR";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::UNUSED:		return "UNUSED";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer m_ObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING || inLayer2 == BroadPhaseLayers::DEBRIS;
		case Layers::MOVING:
			return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING || inLayer2 == BroadPhaseLayers::SENSOR;
		case Layers::DEBRIS:
			return inLayer2 == BroadPhaseLayers::NON_MOVING;
		case Layers::SENSOR:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::UNUSED1:
		case Layers::UNUSED2:
		case Layers::UNUSED3:
			return false;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};
