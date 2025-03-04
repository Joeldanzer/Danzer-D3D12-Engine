#pragma once
#include "Core/MathDefinitions.h"
#include "Components/ComponentRegister.h"

struct SpotLight : public BaseComponent {
	float m_range;
	float m_radius;

	Vect3f m_offset;
};
REGISTER_COMPONENT(SpotLight)