#pragma once
#include "Collider.h"

class Transform;
class AABBCollider;
class SphereCollider;
struct Object;

class RayCollider : public Collider
{
public:
	RayCollider() : m_direction(0.0f, 0.0f, 0.0f), m_distance(1.0f), m_invDirection(0.0f, 0.0f, 0.0f) {}
	RayCollider(Vect3f origin, Vect3f destination, Vect3f offset = {0.f, 0.f, 0.f});
	RayCollider(Vect3f origin, Vect3f direction, float distance, Vect3f offset = {0.f, 0.f, 0.f});
	
	Vect3f m_direction;
	float m_distance;	

private:
	friend class CollisionManager;

	void InitializeRay();

	Vect3f m_invDirection;
};

inline RayCollider::RayCollider(Vect3f origin, Vect3f destination, Vect3f offset) :
	m_distance(DistanceVect3(origin + offset, destination)),
	m_direction(DirectionBetween2Points(origin + offset, destination))
{
	m_offset = offset;
	m_position = origin + offset;
	InitializeRay();
}

inline RayCollider::RayCollider(Vect3f origin, Vect3f direction, float distance, Vect3f offset) :
	m_distance(distance),
	m_direction(direction)
{
	m_offset = offset;
	m_position = origin + offset;
	InitializeRay();
}

inline void RayCollider::InitializeRay()
{
	m_invDirection = { 1.f / m_direction.x, 1.f / m_direction.y, 1.f / m_direction.z };
}


