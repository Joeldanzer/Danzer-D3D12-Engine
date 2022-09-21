#pragma once
#include "Core/MathDefinitions.h"

struct Transform2D {
	Vect2f m_position;
	Vect2f m_scale;

private:
	Transform2D* m_parent = nullptr;
};