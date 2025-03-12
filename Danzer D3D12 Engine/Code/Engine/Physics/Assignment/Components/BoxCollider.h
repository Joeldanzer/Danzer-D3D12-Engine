#pragma once
#include "Components/ComponentRegister.h"
#include "Physics/Assignment/SimplePhysics.h"

struct BoxCollider : public BaseComponent, public BaseCollider {
	COMP_FUNC(BoxCollider)
	
	BoxCollider() : 
		m_extents(1.0f, 1.0f, 1.0f)
	{}
	BoxCollider(const Vect3f extents) : 
		m_extents(extents)
	{}

	Vect3f m_extents;

	void DisplayInEditor(const Entity entity) override;
};
REGISTER_COMPONENT(BoxCollider)