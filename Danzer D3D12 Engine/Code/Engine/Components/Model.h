#pragma once
#include "Components/ComponentRegister.h"

typedef uint32_t ModelID;

// Model Component. Required to be able to render models in scene.
struct Model : public BaseComponent {
	COMP_FUNC(Model)

	Model()			   : m_modelID(UINT32_MAX) {}
	Model(ModelID id) : m_modelID(id) {}

	void operator=(const ModelID modelID) {
		m_modelID = modelID;
	}
	void operator=(const Model& model) {
		m_modelID = model.m_modelID;
	}
	void DisplayInEditor(const Entity entity) override;

	void WriteComponentToFile(const Entity entity, std::fstream& file) override;
	void LoadFileToComponent(const Entity entity, std::fstream& file) override;

	ModelID m_modelID;
};
REGISTER_COMPONENT(Model)