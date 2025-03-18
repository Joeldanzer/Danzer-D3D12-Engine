#pragma once
#include "Core/MathDefinitions.h"
#include "Core/RegistryWrapper.h"

struct ARay {
	ARay() = delete;
	ARay(const Vect3f& origin, const Vect3f& direction, std::function<void(const Entity, const float)> hit = nullptr) :
		m_origin(origin),
		m_direction(direction),
		m_RayHit(hit)
	{
	}

	// Ray Hit function pointer looks like Const Entity entity, const Vect3f& point, const float distance
	std::function<void(const Entity, const float)> m_RayHit = nullptr;

	const Vect3f m_origin;
	const Vect3f m_direction;
};

class RayCaster
{
public:
	RayCaster(){}
	static RayCaster& GetInstance() {
		if (s_rayCaster == nullptr)
			s_rayCaster = new RayCaster();
		return *s_rayCaster;
	}

	void operator=(RayCaster& other) = delete;
	void operator=(RayCaster* other) = delete;

	void CastRay(const Vect3f& origin, const Vect3f& direction, std::function<void(const Entity, const float)> hit = nullptr);
	void CastRay(const ARay& ray);

private:
	friend class SimplePhysics;

	static RayCaster* s_rayCaster;

	std::vector<ARay> m_rays;
};

