#pragma once
#include "Core/MathDefinitions.h"

struct Transform2D {
	Vect2f m_position = { 0.f, 0.f };
	Vect2f m_scale	  = { 1.f, 1.f };

private:
	Transform2D* m_parent = nullptr;
};