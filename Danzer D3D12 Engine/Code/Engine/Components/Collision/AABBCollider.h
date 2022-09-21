#pragma once
#include "Collider.h"

class AABBCollider : public Collider {
public:
	AABBCollider(){}
	AABBCollider(float width, float height, float depth, Vect3f position, Vect3f offset = { 0.f, 0.f, 0.f });
	AABBCollider(Vect3f min, Vect3f max, Vect3f offset = { 0.f, 0.f, 0.f });
	~AABBCollider() {}

	float m_width;
	float m_height;
	float m_depth;

private:
	friend class CollisionManager;
	void UpdateMinAndMax();

	Vect3f m_min;
	Vect3f m_max;
};

inline AABBCollider::AABBCollider(float width, float height, float depth, Vect3f position, Vect3f offset) :
	m_width(width),
	m_height(height),
	m_depth(depth)
{
	m_offset = offset;
	m_position = offset + position;
	UpdateMinAndMax();
}
inline AABBCollider::AABBCollider(Vect3f min, Vect3f max, Vect3f offset)
{
	m_position = (max - min);
	m_position /= 2.f;

	m_width = m_position.x;
	m_height = m_position.y;
	m_depth = m_position.z;

	m_offset = offset;
	m_position += min + offset;
	UpdateMinAndMax();
}

inline void AABBCollider::UpdateMinAndMax()
{
	m_max = { m_position.x + (m_width / 2.f), m_position.y + (m_height / 2.f), m_position.z + (m_depth / 2.f) };
	m_min = { m_position.x - (m_width / 2.f), m_position.y - (m_height / 2.f), m_position.z - (m_depth / 2.f) };
}
