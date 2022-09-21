#pragma once
#include "Core/MathDefinitions.h"

struct Sprite{
	unsigned int m_spriteSheet = 0;
	unsigned int m_frame = 0;

	Vect2f m_pixelOffset = { 0.f, 0.f };
	Vect2f m_anchor = { 0.f, 0.f };
	Vect2f m_size = { 1.f, 1.f };
};