#pragma once
#include "Core/MathDefinitions.h"
#include "Components/ComponentRegister.h"

struct PointLight : public BaseComponent 
{
	COMP_FUNC(PointLight)

	PointLight() :
		m_color(1.0f, 1.0f, 1.0f, 1.0f),
		m_range(1.0f),
		m_offset()
	{}
	PointLight(Colorf color, float range, Vect3f offset = Vect3f::Zero) :
		m_color(color),
		m_range(range),
		m_offset(offset)
	{}

	void DisplayInEditor(Entity entity) override;

	Colorf m_color;
	Vect3f m_offset;
	float  m_range;
};
REGISTER_COMPONENT(PointLight);