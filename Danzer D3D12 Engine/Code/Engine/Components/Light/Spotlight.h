#pragma once
#include "Core/MathDefinitions.h"
#include "Components/ComponentRegister.h"

struct SpotLight : public BaseComponent {
	COMP_FUNC(SpotLight)

	SpotLight(){}
	SpotLight(const Colorf color, const float range = 5.0f, const float cutOff = 0.1f, const float outerCutOff = 0.1f) : 
		m_color(color),
		m_range(range),
		m_cutOff(cutOff),
		m_outerCutOff(outerCutOff)
	{}

	void DisplayInEditor(const Entity entity) override;

	Colorf m_color		= { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_range       = 5.0f;
	float m_cutOff      = 0.1f;
	float m_outerCutOff = 0.1f;

	Vect3f m_offset;
};
REGISTER_COMPONENT(SpotLight)