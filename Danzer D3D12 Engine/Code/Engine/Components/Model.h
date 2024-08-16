#pragma once

// Model Component. Required to be able to render models in scene.
struct Model {
	Model(unsigned int id) : m_modelID(id){}
	unsigned int m_modelID;
};