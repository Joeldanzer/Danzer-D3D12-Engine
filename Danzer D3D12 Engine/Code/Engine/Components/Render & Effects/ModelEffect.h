#pragma once
#include "Components/ComponentRegister.h"

struct ModelEffect : public BaseComponent {
	ModelEffect() : m_effectID(0) {}
	ModelEffect(const unsigned int id)
		: m_effectID(id)
	{};

	unsigned int m_effectID;
};
REGISTER_COMPONENT(ModelEffect)