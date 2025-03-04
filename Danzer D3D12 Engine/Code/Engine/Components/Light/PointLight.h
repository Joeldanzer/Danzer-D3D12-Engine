#pragma once
#include "Core/MathDefinitions.h"
#include "Components/ComponentRegister.h"

struct PointLight : public BaseComponent {
	PointLight() :
		m_color(1.0f, 1.0f, 1.0f, 1.0f),
		m_range(0.0f),
		m_offset()
	{}
	PointLight(Colorf color, float range, Vect3f offset = Vect3f::Zero) :
		m_color(color),
		m_range(range),
		m_offset(offset)
	{}

	Colorf m_color;
	float  m_range;
	Vect3f m_offset;
};
REGISTER_COMPONENT(PointLight)