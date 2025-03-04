#pragma once
#include "Core/MathDefinitions.h"

struct Transform;
struct Camera;
class  DebugRenderingData;

class FrustrumCulling
{
public:
	struct Plane {
		Plane(){}
		Plane(const Vect3f& pos, const Vect3f& norm) : 
			m_normal(DirectX::XMVector3Normalize(norm)),
			m_distance(Dot(m_normal, pos))
		{}
		Vect3f m_normal   = { 0.0f, 1.0f, 0.0f };
		float  m_distance = 0.0f;
	};
	
	struct AABB {
		AABB() : m_center(), m_extent() {}
		AABB(const Vect3f& min, const Vect3f& max) :
			m_center((max + min) * 0.5f),
			m_extent(max.x - m_center.x, max.y - m_center.y, max.z - m_center.z)
		{}
		AABB(const Vect3f& center, const float iL, const float iJ, const float iK) :
			m_center(center),
			m_extent(iL, iJ, iK)
		{}

		Vect3f m_center;
		Vect3f m_extent;

		bool IsOnForwardPlane(const Planef& plane) const  {
			float d = m_center.Dot(plane.Normal()); // fetch dot value of the center vs the plane normal 
			float r = m_extent.Dot({ std::fabs(plane.Normal().x), std::fabs(plane.Normal().y), std::fabs(plane.Normal().z) }); // fetch dot product of AABB Extent & Planes Abs normal

			// add the two distances together and compare their distance against -plane.Distance()
			if (d + r <  -plane.D()) // Fully inside
				return true;
			if (d - r <= -plane.D()) // partially inside
				return true;

			return false;
		}
	};

	static bool MeshIsInFrustrum(const Camera& cam, Transform& transform, const AABB& aabb, DebugRenderingData& debug);
};
