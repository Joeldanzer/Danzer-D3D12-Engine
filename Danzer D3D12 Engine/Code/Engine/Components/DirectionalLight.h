#pragma once
#include "Core\MathDefinitions.h"

struct DirectionalLight
{
	DirectionalLight(){}
	DirectionalLight(Vect4f light, Vect4f ambient, Vect3f positon = {0.0f, 100.0f, 0.f}) :
		m_lightColor(light),
		m_ambientColor(ambient),
		m_lightPosition(positon)
	{}

	void SetShadowProjection(const float scale, const float increase) {
		m_lightProjection = DirectX::XMMatrixOrthographicLH(scale, scale, -(scale * increase), scale * increase);
	}

	Vect3f m_lightPosition;
	Vect4f m_lightColor;
	Vect4f m_ambientColor;

	Mat4f  m_lightTransform;
	Mat4f  m_lightProjection;
};

