#include "stdafx.h"

#include "ModelHandler.h"

#include "../3rdParty/assimp-master/include/assimp/Scene.h"

#include "../RenderManager.h"
#include "../RenderUtility.h"
#include "../../Core/DirectX12Framework.h"

#include "Scene.h"

#include <iostream>
#include <filesystem>

ModelHandler::~ModelHandler(){
	m_models.clear();
}

Model ModelHandler::CreateCustomModel(CustomModel customModel, std::wstring texturePath, bool transparent)
{
	ID3D12Device* device = m_framework.GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_framework.GetCommandList();
	// Check if this model has already been loaded, and if so just return its ID
	UINT modelExist = GetExistingModel(customModel.m_customModelName);
	if (modelExist != 0) {
		return modelExist;
	}
	
	m_framework.ResetCommandListAndAllocator(nullptr);
	
	std::vector<CD3DX12_RESOURCE_BARRIER> resourceBarriers;
	VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(sizeof(Vertex) * (unsigned int)customModel.m_verticies.size(), 
		sizeof(unsigned int) * (unsigned int)customModel.m_indices.size(), device);
	
	ModelData::Mesh mesh;
	resourceBarriers.emplace_back(SetSubresourceData(
		cmdList,
		bufferInfo.m_vBuffer,
		bufferInfo.m_vBufferUpload,
		reinterpret_cast<UINT*>(&customModel.m_verticies[0]),
		sizeof(Vertex), static_cast<UINT>(customModel.m_verticies.size())));
	
	resourceBarriers.emplace_back(SetSubresourceData(
		cmdList,
		bufferInfo.m_iBuffer,
		bufferInfo.m_iBufferUpload,
		reinterpret_cast<UINT*>(&customModel.m_indices[0]),
		sizeof(UINT), static_cast<UINT>(customModel.m_indices.size())));
	
	mesh.m_vertexBuffer = bufferInfo.m_vBuffer;
	mesh.m_vertexSize = sizeof(Vertex);
	mesh.m_numVerticies = static_cast<UINT>(customModel.m_verticies.size());
	
	mesh.m_indexBuffer = bufferInfo.m_iBuffer;
	mesh.m_numIndices = static_cast<UINT>(customModel.m_indices.size());
	
	//Faster to execute them at the same time
	cmdList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);
	
	
	m_framework.ExecuteCommandList();
	m_framework.WaitForPreviousFrame();
	
	std::vector<Vect3f> verticies;

	// Now add the data from the GPU -> CPU after executinng commandlist
	{
		mesh.m_indexBufferView.BufferLocation = mesh.m_indexBuffer->GetGPUVirtualAddress();
		mesh.m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		mesh.m_indexBufferView.SizeInBytes = sizeof(unsigned int) * mesh.m_numIndices;
		
		mesh.m_vertexBufferView.BufferLocation = mesh.m_vertexBuffer->GetGPUVirtualAddress();
		mesh.m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		mesh.m_vertexBufferView.SizeInBytes = sizeof(Vertex) * mesh.m_numVerticies;
	}

	for (UINT i = 0; i < customModel.m_verticies.size(); i++)
	{
		Vect3f pos = { customModel.m_verticies[i].m_position.x, customModel.m_verticies[i].m_position.y, customModel.m_verticies[i].m_position.z };
		verticies.emplace_back(pos);
	}
	
	UINT texture = m_textureHandler.CreateTexture(texturePath);
	m_textureHandler.LoadAllCreatedTexuresToGPU();

	std::vector<ModelData::Mesh> meshes = { mesh };
	UINT id = GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), verticies, customModel.m_customModelName, transparent));
	
	return Model(id);
}

// Load model from Assimp, if you need to get a model ID 
// Use GetExistingModel(srtring name) instead. 
Model ModelHandler::LoadModel(std::wstring fileName, std::string name, bool transparent, bool uvFlipped)
{
	//std::string nameStr(fileName.begin(), fileName.end());
	
	UINT exists = GetExistingModel(name);
	if (exists != 0) {
		return exists;
	}
	
	// Always restet CommandLists as executing them requires them to be close
	m_framework.ResetCommandListAndAllocator(nullptr);

	std::string modelStr = { fileName.begin(), fileName.end() };
	std::unique_ptr<LoaderModel> loadedModel = m_modelLoader.LoadModelFromAssimp(modelStr, uvFlipped);

	std::vector<ModelData::Mesh> meshes = LoadMeshFromLoaderModel(loadedModel.get(), modelStr);
	std::vector<Vect3f> verticies = loadedModel->m_verticies;

	// Now we set the buffer infromation into our BUFFER_VIEWS as well as 
	// creating DescriptorHeaps and SRV for our textures
	for (UINT i = 0; i < meshes.size(); i++)
	{
		meshes[i].m_indexBufferView.BufferLocation = meshes[i].m_indexBuffer->GetGPUVirtualAddress();
		meshes[i].m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		meshes[i].m_indexBufferView.SizeInBytes = sizeof(unsigned int) * meshes[i].m_numIndices;

		meshes[i].m_vertexBufferView.BufferLocation = meshes[i].m_vertexBuffer->GetGPUVirtualAddress();
		meshes[i].m_vertexBufferView.StrideInBytes = meshes[i].m_vertexSize;
		meshes[i].m_vertexBufferView.SizeInBytes =   meshes[i].m_vertexSize * meshes[i].m_numVerticies;
	} 
	
	m_textureHandler.LoadAllCreatedTexuresToGPU();

	std::string modelName = "";
	if (name.empty())
		modelName = SetModelName(fileName);
	else
		modelName = name;

	UINT id = GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), verticies, modelName, transparent));
	return Model(id);
}

Model ModelHandler::LoadModel(LoaderModel* loadedModel, bool transparent, bool uvFlipped)
{
	UINT exists = GetExistingModel(loadedModel->m_name);
	if (exists != 0) {
		return exists;
	}

	m_framework.ResetCommandListAndAllocator(nullptr);
	std::string s = SetModelName({ loadedModel->m_name.begin(), loadedModel->m_name.end()});
	
	std::vector<ModelData::Mesh> meshes = LoadMeshFromLoaderModel(loadedModel, s);

	// Now we set the buffer infromation into our BUFFER_VIEWS as well as 
	// creating DescriptorHeaps and SRV for our textures
	for (UINT i = 0; i < meshes.size(); i++)
	{
		meshes[i].m_indexBufferView.BufferLocation = meshes[i].m_indexBuffer->GetGPUVirtualAddress();
		meshes[i].m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		meshes[i].m_indexBufferView.SizeInBytes = sizeof(unsigned int) * meshes[i].m_numIndices;

		meshes[i].m_vertexBufferView.BufferLocation = meshes[i].m_vertexBuffer->GetGPUVirtualAddress();
		meshes[i].m_vertexBufferView.StrideInBytes = meshes[i].m_vertexSize;
		meshes[i].m_vertexBufferView.SizeInBytes = meshes[i].m_vertexSize * meshes[i].m_numVerticies;
	}

	m_textureHandler.LoadAllCreatedTexuresToGPU();

	UINT id = GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), loadedModel->m_verticies, s, transparent));
	return Model(id);
}

UINT ModelHandler::GetExistingModel(std::string modelName)
{
	if(modelName.empty())
		modelName = SetModelName({ modelName.begin(), modelName.end() }); 

	for (UINT i = 0; i < m_models.size(); i++)
	{
		if (modelName == m_models[i].Name()) {
			return i + 1;
		}
	}

	return 0;
}

UINT ModelHandler::LoadModelFromLevel(LoaderModel* loadedModel, std::vector<UINT>& textures, bool transparent, bool uvFlipped)
{

	UINT exists = GetExistingModel(loadedModel->m_name);
	if (exists != 0) {
		return exists;
	}

	m_framework.ResetCommandListAndAllocator(nullptr);
	std::vector<ModelData::Mesh> meshes = LoadMeshFromLoaderModel(loadedModel, textures);

	std::string s = SetModelName({ loadedModel->m_name.begin(), loadedModel->m_name.end() });

	// Now we set the buffer infromation into our BUFFER_VIEWS as well as 
	// creating DescriptorHeaps and SRV for our textures
	for (UINT i = 0; i < meshes.size(); i++)
	{
		meshes[i].m_indexBufferView.BufferLocation = meshes[i].m_indexBuffer->GetGPUVirtualAddress();
		meshes[i].m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		meshes[i].m_indexBufferView.SizeInBytes = sizeof(unsigned int) * meshes[i].m_numIndices;

		meshes[i].m_vertexBufferView.BufferLocation = meshes[i].m_vertexBuffer->GetGPUVirtualAddress();
		meshes[i].m_vertexBufferView.StrideInBytes = meshes[i].m_vertexSize;
		meshes[i].m_vertexBufferView.SizeInBytes = meshes[i].m_vertexSize * meshes[i].m_numVerticies;

	}

	UINT id = GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), loadedModel->m_verticies, s, transparent));
	
	return id;
}



void ModelHandler::SetAlbedoForModel(UINT model, std::vector<std::string> textures)
{
	ModelData& data = m_models[model - 1];
	data.m_albedoTextures = m_textureHandler.CreateMultipleTextures(&textures[0], data.m_meshes.size());
}

void ModelHandler::SetNormalForModel(UINT model, std::vector<std::string> textures)
{
	ModelData& data = m_models[model - 1];
	data.m_normalTextures = m_textureHandler.CreateMultipleTextures(&textures[0], data.m_meshes.size());
}

void ModelHandler::SetMaterialForModel(UINT model, std::vector<std::string> textures)
{
	ModelData& data = m_models[model - 1];
	data.m_materialTextures = m_textureHandler.CreateMultipleTextures(&textures[0], data.m_meshes.size());
	data.m_hasMaterialTextures = true;
}

void ModelHandler::SetMaterialValues(UINT model, float metallic, float roughness, float emissive)
{
	ModelData& data = m_models[model - 1];
	data.m_material.m_emissive = emissive;
	data.m_material.m_metallic = metallic;
	data.m_material.m_roughnes = roughness;
}

UINT ModelHandler::GetNewlyCreatedModelID(ModelData model)
{
	UINT id = 0;

	for (UINT i = 0; i < m_models.size(); i++)
	{
		// If the model is found don't add it to the list
		if (model.Name() == m_models[i].Name()) {
			id = m_models[i].GetID();
			return id;
		}
	}

	m_models.emplace_back(model);
	id = (UINT)m_models.size();
	m_models[id - 1].SetID(id);

	return id;
}

std::vector<ModelData::Mesh> ModelHandler::LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::string name)
{
	ID3D12Device* device = m_framework.GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_framework.GetCommandList();

	std::vector<ModelData::Mesh> meshes;
	meshes.reserve(loadedModel->m_meshes.size());

	std::vector<CD3DX12_RESOURCE_BARRIER> resourceBarriers;

	for (UINT i = 0; i < loadedModel->m_meshes.size(); i++)
	{
		ModelData::Mesh mesh;
		LoaderMesh* loadedMesh = loadedModel->m_meshes[i];

		// Load our Buffers and Upload Buffers 
		VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(
			loadedMesh->m_vertexSize * loadedMesh->m_vertexCount,
			static_cast<UINT>(loadedMesh->m_indices.size()) * sizeof(UINT),
			device);


		// Set upload Vertex Buffer SubResourceData to our vertex destBuffer
		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, bufferInfo.m_vBuffer, bufferInfo.m_vBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_verticies), loadedMesh->m_vertexSize, loadedMesh->m_vertexCount
		));

		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, bufferInfo.m_iBuffer, bufferInfo.m_iBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_indices.data()), sizeof(UINT), static_cast<UINT>(loadedMesh->m_indices.size())
		));

		// Upload the newly transitioned Resource Barriers to our cmdList
		mesh.m_indexBuffer  = bufferInfo.m_iBuffer;
		mesh.m_vertexBuffer = bufferInfo.m_vBuffer;
		mesh.m_numIndices   = static_cast<UINT>(loadedMesh->m_indices.size());
		mesh.m_numVerticies = loadedMesh->m_vertexCount;
		mesh.m_vertexSize   = loadedMesh->m_vertexSize;

		// Add our newly created model mesh
		meshes.emplace_back(mesh);
	}

	cmdList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);
	// Execute our commandList so the new information is sent to the GPU
	m_framework.ExecuteCommandList();
	m_framework.WaitForPreviousFrame();

	return meshes;
}

std::vector<ModelData::Mesh> ModelHandler::LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::vector<UINT>& textures)
{
	ID3D12Device* device = m_framework.GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_framework.GetCommandList();

	std::vector<ModelData::Mesh> meshes;
	meshes.reserve(loadedModel->m_meshes.size());

	std::vector<CD3DX12_RESOURCE_BARRIER> resourceBarriers;

	for (UINT i = 0; i < loadedModel->m_meshes.size(); i++)
	{
		ModelData::Mesh mesh;
		LoaderMesh* loadedMesh = loadedModel->m_meshes[i];

		// Load our Buffers and Upload Buffers 
		VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(
			loadedMesh->m_vertexSize * loadedMesh->m_vertexCount,
			static_cast<UINT>(loadedMesh->m_indices.size()) * sizeof(UINT),
			device);


		// Set upload Vertex Buffer SubResourceData to our vertex destBuffer
		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, bufferInfo.m_vBuffer, bufferInfo.m_vBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_verticies), loadedMesh->m_vertexSize, loadedMesh->m_vertexCount
		));

		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, bufferInfo.m_iBuffer, bufferInfo.m_iBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_indices.data()), sizeof(UINT), static_cast<UINT>(loadedMesh->m_indices.size())
		));

		// Upload the newly transitioned Resource Barriers to our cmdList
		mesh.m_indexBuffer  = bufferInfo.m_iBuffer;
		mesh.m_vertexBuffer = bufferInfo.m_vBuffer;
		mesh.m_numIndices   = static_cast<UINT>(loadedMesh->m_indices.size());
		mesh.m_numVerticies = loadedMesh->m_vertexCount;
		mesh.m_vertexSize   = loadedMesh->m_vertexSize;

		// Add our newly created model mesh
		meshes.emplace_back(mesh);
	}

	cmdList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);
	// Execute our commandList so the new information is sent to the GPU
	m_framework.ExecuteCommandList();
	m_framework.WaitForPreviousFrame();

	return meshes;
}

std::string ModelHandler::SetModelName(std::wstring modelPath)
{
	std::string result(modelPath.begin(), modelPath.end());

	size_t firstPos;
	firstPos = result.find_last_of("/");
	if (firstPos != std::string::npos) {
		firstPos = result.find_last_of("/");
		
		result.replace(0, firstPos + 1, "");
		
		size_t lastPos = result.find_first_of(".");
		result.replace(lastPos, lastPos + 4, "");
	}

	return result;
}
