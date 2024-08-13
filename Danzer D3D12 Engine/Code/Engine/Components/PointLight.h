#pragma once
#include "Core/MathDefinitions.h"

struct PointLight {
	PointLight() :
		m_id(0),
		m_color(),
		m_range(0.0f),
		m_offsetPosition()
	{}
	//PointLight(const UINT id, Vect3f color, float intensity, float range) : 
	//	m_id(id),
	//	m_color(color.x, color.y, color.z, intensity),
	//	m_range(range),
	//	m_offsetPosition()
	//{}
	
	PointLight(const UINT id) :
		m_id(id),
		m_color(),
		m_range(0.0f),
		m_offsetPosition()
	{}

	Vect4f m_color;
	float  m_range;

	Vect3f m_offsetPosition;

private:
	friend class LightHandler; 
	UINT m_id = -1;
};