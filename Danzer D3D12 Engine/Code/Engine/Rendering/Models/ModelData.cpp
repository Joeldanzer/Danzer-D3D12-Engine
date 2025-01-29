#include "stdafx.h"
#include "ModelData.h"

CustomModel ModelData::GetCube()
{
	CustomModel model;
	model.m_customModelName = "Cube";
	model.m_verticies = {
	// Position:         Color:     UV:
	//Front face
	{ {-1, -1,  1,  1,}, {1, 1}},
	{ { 1, -1,  1,  1,}, {1, 0}},
	{ { 1,  1,  1,  1,}, {0, 0}},
	{ {-1,  1,  1,  1,}, {0, 1}},
						  
	//Back face			 
	{ {-1, -1, -1,  1,}, {1, 1}},
	{ {-1,  1, -1,  1,}, {1, 0}},
	{ { 1,  1, -1,  1,}, {0, 0}},
	{ { 1, -1, -1,  1,}, {0, 1}},
						 
	//Top face			 
	{ {-1,  1, -1,  1,}, {1, 1}},
	{ {-1,  1,  1,  1,}, {1, 0}},
	{ { 1,  1,  1,  1,}, {0, 0}},
	{ { 1,  1, -1,  1,}, {0, 1}},
						 
	//Bottom face		 
	{ {-1, -1, -1,  1,}, {1, 1}},
	{ { 1, -1, -1,  1,}, {1, 0}},
	{ { 1, -1,  1,  1,}, {0, 0}},
	{ {-1, -1,  1,  1,}, {0, 1}},
						 
	//Right face		 
	{ { 1, -1, -1,  1,}, {1, 1}},
	{ { 1,  1, -1,  1,}, {1, 0}},
	{ { 1,  1,  1,  1,}, {0, 0}},
	{ { 1, -1,  1,  1,}, {0, 1}},
						 
	//Left face			 
	{ {-1, -1, -1,  1,}, {1, 1}},
	{ {-1, -1,  1,  1,}, {1, 0}},
	{ {-1,  1,  1,  1,}, {0, 0}},
	{ {-1,  1, -1,  1,}, {0, 1}}};
	
	model.m_indices = {
		 0,  1,  2,      0,  2,  3,    // Front
		 4,  5,  6,      4,  6,  7,    // Back
		 8,  9, 10,      8, 10, 11,    // Top
		12, 13, 14,     12, 14, 15,    // Bottom
		16, 17, 18,     16, 18, 19,    // Right
		20, 21, 22,     20, 22, 23,    // Left
	};
	
	return model;
}

CustomModel ModelData::GetPlane()
{
	CustomModel model;
	model.m_customModelName = "Plane";
	model.m_verticies = {
		// Position:          UV:
		{ {-1,  1,  1,  1,}, {0, 0}},
		{ { 1, -1,  1,  1,}, {1, 1}},
		{ {-1, -1,  1,  1,}, {0, 1}},
		{ {-1,  1,  1,  1,}, {0, 0}},
		{ { 1,  1,  1,  1,}, {1, 0}},
		{ { 1, -1,  1,  1,}, {1, 1}},
	};

	model.m_indices = {
		0, 1, 2, 3, 4, 5
	};
	
	return model;
}

// Clears all instance transform on meshes/models.
void ModelData::ClearInstanceTransform() 
{
	for (uint32_t i = 0; i < m_meshes.size(); i++)
		m_meshes[i].m_instanceTransforms.clear();
	
	m_instanceTransforms.clear();
	m_meshToRender.clear();
}

void ModelData::UpdateAllMeshInstanceBuffer(uint32_t frameIndex)
{
	// Doing it this method means we can render transparent meshes seperatly
	for (uint32_t i = 0; i < m_meshes.size(); i++)
	{
		Mesh& mesh = m_meshes[i];
		if(!mesh.m_instanceTransforms.empty())
			mesh.m_meshBuffer.UpdateBuffer(reinterpret_cast<uint16_t*>(&mesh.m_instanceTransforms[0]), (uint32_t)mesh.m_instanceTransforms.size(), frameIndex);
	}
}
