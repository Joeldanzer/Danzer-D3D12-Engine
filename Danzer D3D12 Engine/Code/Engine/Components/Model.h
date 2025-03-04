#pragma once
#include "Components/ComponentRegister.h"

// Model Component. Required to be able to render models in scene.
struct Model : public BaseComponent {
	Model()			   : m_modelID(UINT32_MAX) {}
	Model(uint32_t id) : m_modelID(id) {}

	uint32_t m_modelID;
};
REGISTER_COMPONENT(Model)