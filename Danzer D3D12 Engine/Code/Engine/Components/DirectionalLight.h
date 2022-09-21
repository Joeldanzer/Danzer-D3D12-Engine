#pragma once
#include "Core\MathDefinitions.h"

class DirectionalLight
{
public:
	DirectionalLight(){}
	DirectionalLight(Vect4f light, Vect4f ambient) :
		m_lightColor(light),
		m_ambientColor(ambient)
	{}

	Vect4f m_lightColor;
	Vect4f m_ambientColor;

private:
	// Used for shadows, will use later on
	Mat4f m_lightProjection;
};

