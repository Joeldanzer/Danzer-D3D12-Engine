#pragma once
#include "Core\MathDefinitions.h"
#include "Components/ComponentRegister.h"

struct DirectionalLight : public BaseComponent
{
	DirectionalLight(){}
	DirectionalLight(Vect4f light, Vect4f ambient, Vect3f positon = {0.0f, 100.0f, 0.f}) :
		m_lightColor(light),
		m_ambientColor(ambient),
		m_lightPosition(positon)
	{}

	void SetShadowProjection(const float scale, const float increase) {
		//float scaleIncreas = scale * increase;
		//m_lightProjection = DirectX::XMMatrixOrthographicLH(scale, scale, 1.0f, 2.0f);

		//m_lightProjection = DirectX::XMMatrixOrthographicOffCenterLH(scale, -scale, scale, -scale, scale, -scale);
	}

	Vect3f m_lightPosition;
	Vect4f m_lightColor;
	Vect4f m_ambientColor;

	Mat4f  m_lightTransform;

	void DisplayInEditor(Entity entity) override;
};
REGISTER_COMPONENT(DirectionalLight)



