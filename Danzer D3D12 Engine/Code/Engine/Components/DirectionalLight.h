#pragma once
#include "Core\MathDefinitions.h"



struct DirectionalLight
{
	DirectionalLight(){}
	DirectionalLight(Vect4f light, Vect4f ambient) :
		m_lightColor(light),
		m_ambientColor(ambient)
	{}

	Vect4f m_lightColor;
	Vect4f m_ambientColor;
};

