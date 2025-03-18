#include "stdafx.h"
#include "RayCaster.h"

RayCaster* RayCaster::s_rayCaster = nullptr;

void RayCaster::CastRay(const Vect3f& origin, const Vect3f& direction, std::function<void(const Entity, const float)> hit)
{
	m_rays.emplace_back(ARay(origin, direction, hit));
}

void RayCaster::CastRay(const ARay& ray)
{
	m_rays.emplace_back(ray);
}
