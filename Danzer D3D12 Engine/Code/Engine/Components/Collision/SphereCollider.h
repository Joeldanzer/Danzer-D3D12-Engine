#pragma once
#include "Components/Collision/Collider.h"

class SphereCollider : public Collider
{
public:
	float m_radius;

	SphereCollider(){}
	SphereCollider(Vect3f origin, float radius, Vect3f offset = { 0.f, 0.f, 0.f });
	~SphereCollider();

private:
	friend class CollisionManager;
};

inline SphereCollider::SphereCollider(Vect3f origin, float radius, Vect3f offset) :
	m_radius(radius)
{
	m_offset = offset;
	m_position = origin + offset;
}
inline SphereCollider::~SphereCollider(){}


