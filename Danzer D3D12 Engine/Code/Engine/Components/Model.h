#pragma once
#include "Components/ComponentRegister.h"

// Model Component. Required to be able to render models in scene.
struct Model : public BaseComponent {
	COMP_FUNC(Model)

	Model()			   : m_modelID(UINT32_MAX) {}
	Model(uint32_t id) : m_modelID(id) {}

	void operator=(const uint32_t modelID) {
		m_modelID = modelID;
	}
	void operator=(const Model& model) {
		m_modelID = model.m_modelID;
	}

	void DisplayInEditor(const Entity entity) override;

	uint32_t m_modelID;
};
REGISTER_COMPONENT(Model)