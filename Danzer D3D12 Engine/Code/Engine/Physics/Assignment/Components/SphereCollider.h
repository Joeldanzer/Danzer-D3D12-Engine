#pragma once
#include "Components/ComponentRegister.h"
#include "Physics/Assignment/SimplePhysics.h"

struct SphereCollider : public BaseComponent, public BaseCollider {
	COMP_FUNC(SphereCollider)

	SphereCollider() : 
		m_radius(1.0f)
	{}
	SphereCollider(const float radius) :
		m_radius(radius)
	{}

	float m_radius;

	void DisplayInEditor(const Entity entity) override;
};
REGISTER_COMPONENT(SphereCollider)