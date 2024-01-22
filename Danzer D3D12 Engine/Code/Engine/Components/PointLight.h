#pragma once
#include "Core/MathDefinitions.h"

struct PointLight {
	Vect4f m_color;
	float  m_range;

	Vect3f m_offsetPosition;
};