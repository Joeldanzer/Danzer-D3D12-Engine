#include "stdafx.h"

#include "ModelHandler.h"

#include "assimp/Scene.h"

#include "../RenderManager.h"
#include "../RenderUtility.h"
#include "../../Core/D3D12Framework.h"

#include "Core/FrameResource.h"
#include "Scene.h"
#include "Components/Transform.h"

#include <iostream>
#include <filesystem>

ModelHandler::~ModelHandler(){
	m_models.clear();
}

Model ModelHandler::CreateCustomModel(CustomModel customModel, bool transparent)
{
	ID3D12Device* device = m_framework.GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_framework.InitCmdList();

	UINT modelExist = GetExistingModel(L"CustomCube");
	if (modelExist != 0) {
		return modelExist;
	}
	
	 //if(!m_framework.CmdListIsRecording())
	 //	m_framework.ResetCommandListAndAllocator(nullptr, L"ModelHandler: Line 30");
	
	std::vector<CD3DX12_RESOURCE_BARRIER> resourceBarriers;
	VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(ToWstring(customModel.m_customModelName), sizeof(Vertex) * (unsigned int)customModel.m_verticies.size(),
		sizeof(unsigned int) * (unsigned int)customModel.m_indices.size(), device);
	
	ModelData::Mesh mesh;
	resourceBarriers.emplace_back(SetSubresourceData(
		cmdList,
		bufferInfo.m_vBuffer,
		bufferInfo.m_vBufferUpload,
		reinterpret_cast<UINT*>(&customModel.m_verticies[0]),
		sizeof(Vertex), static_cast<UINT>(customModel.m_verticies.size()), 
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		));
	
	resourceBarriers.emplace_back(SetSubresourceData(
		cmdList,
		bufferInfo.m_iBuffer,
		bufferInfo.m_iBufferUpload,
		reinterpret_cast<UINT*>(&customModel.m_indices[0]),
		sizeof(UINT), static_cast<UINT>(customModel.m_indices.size()),
		D3D12_RESOURCE_STATE_INDEX_BUFFER
	));
	
	mesh.m_vertexBuffer = bufferInfo.m_vBuffer;
	mesh.m_vertexSize   = sizeof(Vertex);
	mesh.m_numVerticies = static_cast<UINT>(customModel.m_verticies.size());
	
	mesh.m_indexBuffer = bufferInfo.m_iBuffer;
	mesh.m_numIndices  = static_cast<UINT>(customModel.m_indices.size());
	
	//Faster to execute them at the same time
	cmdList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);
	
	std::vector<Vect3f> verticies;

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

	std::vector<ModelData::Mesh> meshes = { mesh };
	UINT id = GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), &m_framework.CbvSrvHeap(), verticies, L"", customModel.m_customModelName, transparent));
	
	return Model(id);
}

Model ModelHandler::LoadModel(std::wstring fileName, std::string name, bool transparent, bool uvFlipped)
{	
	UINT exists = GetExistingModel(fileName);
	if (exists != 0) {
		return exists;
	}
	
	// Always restet CommandLists as executing them requires them to be close
	//m_framework.InitiateCommandList(nullptr, L"ModelHandler Line " + __LINE__);

	std::string modelStr = { fileName.begin(), fileName.end() };
	std::unique_ptr<LoaderModel> loadedModel = m_modelLoader.LoadModelFromAssimp(modelStr, uvFlipped);

	std::vector<ModelData::Mesh> meshes = LoadMeshFromLoaderModel(loadedModel.get(), modelStr);
	std::vector<Vect3f>	      verticies = loadedModel->m_verticies;

	// Now we set the buffer infromation into our BUFFER_VIEWS as well as 
	// creating DescriptorHeaps and SRV for our textures
	for (UINT i = 0; i < meshes.size(); i++)
	{
		meshes[i].m_indexBufferView.BufferLocation = meshes[i].m_indexBuffer->GetGPUVirtualAddress();
		meshes[i].m_indexBufferView.SizeInBytes    = sizeof(unsigned int) * meshes[i].m_numIndices;
		meshes[i].m_indexBufferView.Format         = DXGI_FORMAT_R32_UINT;

		meshes[i].m_vertexBufferView.BufferLocation = meshes[i].m_vertexBuffer->GetGPUVirtualAddress();
		meshes[i].m_vertexBufferView.StrideInBytes  = meshes[i].m_vertexSize;
		meshes[i].m_vertexBufferView.SizeInBytes    = meshes[i].m_vertexSize * meshes[i].m_numVerticies;

	} 

	std::string modelName = "";
	if (name.empty())
		modelName = SetModelName(fileName);
	else
		modelName = name;

	UINT id = GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), &m_framework.CbvSrvHeap(), verticies, fileName, modelName, transparent));
	return Model(id);
}

void ModelHandler::LoadModelsToScene(entt::registry& reg, std::wstring fileName, std::string name, bool uvFlipped)
{
	std::string modelStr = { fileName.begin(), fileName.end() };
	
	std::vector<std::pair<std::string, Mat4f>> transforms;
	std::vector<std::unique_ptr<LoaderModel>>  models = m_modelLoader.LoadMultipleModelsFromAssimp(modelStr, transforms, uvFlipped);

	for (int i = 0; i < models.size(); i++)
	{
		std::vector<ModelData::Mesh> meshes = LoadMeshFromLoaderModel(models[i].get(), modelStr);
		std::vector<Vect3f>	      verticies = models[i]->m_verticies;

		bool isTransparent = models[i]->m_isTransparent;
		// Now we set the buffer infromation into our BUFFER_VIEWS as well as 
		// creating DescriptorHeaps and SRV for our textures
		for (UINT i = 0; i < meshes.size(); i++)
		{
			meshes[i].m_indexBufferView.BufferLocation = meshes[i].m_indexBuffer->GetGPUVirtualAddress();
			meshes[i].m_indexBufferView.SizeInBytes = sizeof(unsigned int) * meshes[i].m_numIndices;
			meshes[i].m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

			meshes[i].m_vertexBufferView.BufferLocation = meshes[i].m_vertexBuffer->GetGPUVirtualAddress();
			meshes[i].m_vertexBufferView.StrideInBytes  = meshes[i].m_vertexSize;
			meshes[i].m_vertexBufferView.SizeInBytes    = meshes[i].m_vertexSize * meshes[i].m_numVerticies;
		}

		std::string modelName = "";
		if (name.empty())
			modelName = SetModelName(fileName);
		else
			modelName = name;

		GetNewlyCreatedModelID(ModelData(meshes, m_framework.GetDevice(), &m_framework.CbvSrvHeap(), verticies, fileName, models[i]->m_name.c_str(), isTransparent));
	}	

	int number = 0;
	for (int i = 0; i < transforms.size(); i++)
	{
		for (int j = 0; j < m_models.size(); j++)
		{
			if (transforms[i].first == m_models[j].m_name) {
				auto entity = reg.create();
				reg.emplace<GameEntity>(entity);
				reg.emplace<Model>(entity, m_models[j].GetID());
				Transform& transform = reg.emplace<Transform>(entity);
				transform.SetTransform(transforms[i].second);
				number++;
			}

		}
	}
}

UINT ModelHandler::GetExistingModel(std::wstring modelPath)
{
	for (UINT i = 0; i < m_models.size(); i++)
	{
		if (modelPath == m_models[i].m_modelPath) {
			return i + 1;
		}
	}

	return 0;
}

void ModelHandler::SetMaterialForModel(UINT model, Material material, UINT meshIndex)
{
	ModelData& modelData = m_models[model - 1]; 
	ModelData::Mesh& mesh = modelData.m_meshes[meshIndex];

	mesh.m_material = material;
}

Material ModelHandler::GetNewMaterialFromLoadedModel(const std::string& materialName)
{
	Material material;
	std::wstring albedo     = std::wstring(materialName.begin(), materialName.end()) + L"_Diffuse.dds";
	std::wstring normal     = std::wstring(materialName.begin(), materialName.end()) + L"_Normal.dds";
	std::wstring metal      = std::wstring(materialName.begin(), materialName.end()) + L"_Metallic.dds";
	std::wstring ao         = std::wstring(materialName.begin(), materialName.end()) + L"_AO.dds";
	std::wstring smoothness = std::wstring(materialName.begin(), materialName.end()) + L"_Smoothness.dds";
	std::wstring height     = std::wstring(materialName.begin(), materialName.end()) + L"_Height.dds";

	// Naming convention on this model is soooo bad that I gotta do this >:(
	if (materialName.find("Fabric_Curtain") != std::string::npos) {
		normal     = L"Sprites/Fabric_Curtain_Normal.dds";
		metal      = L"Sprites/Fabric_Curtain_Metallic.dds";
		ao		   = L"Sprites/Fabric_Curtain_AO.dds";
		smoothness = L"Sprites/Fabric_Curtain_Smoothness.dds";

	}
	else if (materialName.find("Fabric_Round") != std::string::npos) {
		normal     = L"Sprites/Fabric_Round_Normal.dds";
		metal      = L"Sprites/Fabric_Round_Metallic.dds";
		ao         = L"Sprites/Fabric_Round_AO.dds";
		smoothness = L"Sprites/Fabric_Round_Smoothness.dds";
	}

	material.m_albedo	    = m_textureHandler.GetTexture(albedo);
	material.m_normal	    = m_textureHandler.GetTexture(normal);
	material.m_aoMap	    = m_textureHandler.GetTexture(ao);
	material.m_metallicMap  = m_textureHandler.GetTexture(metal);
	material.m_roughnessMap = m_textureHandler.GetTexture(smoothness);
	material.m_heightMap    = m_textureHandler.GetTexture(height);

	return material;
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
	ID3D12GraphicsCommandList* cmdList = m_framework.InitCmdList();

	std::vector<ModelData::Mesh> meshes;
	meshes.reserve(loadedModel->m_meshes.size());

	std::vector<CD3DX12_RESOURCE_BARRIER> resourceBarriers;

	for (UINT i = 0; i < loadedModel->m_meshes.size(); i++)
	{
		ModelData::Mesh mesh;
		LoaderMesh* loadedMesh = loadedModel->m_meshes[i];

		// Load our Buffers and Upload Buffers 
		VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(
		    ToWstring(name), 
			loadedMesh->m_vertexSize * loadedMesh->m_vertexCount,
			static_cast<UINT>(loadedMesh->m_indices.size()) * sizeof(UINT),
			device
		);

		// Set upload Vertex Buffer SubResourceData to our vertex destBuffer
		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, 
			bufferInfo.m_vBuffer, 
			bufferInfo.m_vBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_verticies), 
			loadedMesh->m_vertexSize, 
			loadedMesh->m_vertexCount,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		));

		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, 
			bufferInfo.m_iBuffer, 
			bufferInfo.m_iBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_indices.data()), 
			sizeof(UINT), 
			static_cast<UINT>(loadedMesh->m_indices.size()),
			D3D12_RESOURCE_STATE_INDEX_BUFFER
		));

		mesh.m_indexBuffer  = bufferInfo.m_iBuffer;
		mesh.m_vertexBuffer = bufferInfo.m_vBuffer;
		mesh.m_numIndices   = static_cast<UINT>(loadedMesh->m_indices.size());
		mesh.m_numVerticies = loadedMesh->m_vertexCount;
		mesh.m_vertexSize   = loadedMesh->m_vertexSize;

		if (!loadedModel->m_textures.empty())
			mesh.m_material = GetNewMaterialFromLoadedModel(loadedModel->m_textures[i]);
		else
			mesh.m_material = {};
		
		// Add our newly created model mesh
		meshes.emplace_back(mesh);
	}

	cmdList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);

	return meshes;
}

std::vector<ModelData::Mesh> ModelHandler::LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::vector<UINT>& textures)
{
	ID3D12Device* device = m_framework.GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_framework.InitCmdList();

	std::vector<ModelData::Mesh> meshes;
	meshes.reserve(loadedModel->m_meshes.size());

	std::vector<CD3DX12_RESOURCE_BARRIER> resourceBarriers;

	for (UINT i = 0; i < loadedModel->m_meshes.size(); i++)
	{
		ModelData::Mesh mesh;
		LoaderMesh* loadedMesh = loadedModel->m_meshes[i];

		// Load our Buffers and Upload Buffers 
		VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(
			ToWstring(loadedModel->m_name),
			loadedMesh->m_vertexSize * loadedMesh->m_vertexCount,
			static_cast<UINT>(loadedMesh->m_indices.size()) * sizeof(UINT),
			device
		);


		// Set upload Vertex Buffer SubResourceData to our vertex destBuffer
		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, 
			bufferInfo.m_vBuffer, 
			bufferInfo.m_vBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_verticies), 
			loadedMesh->m_vertexSize, 
			loadedMesh->m_vertexCount,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		));

		resourceBarriers.emplace_back(SetSubresourceData(
			cmdList, bufferInfo.m_iBuffer, 
			bufferInfo.m_iBufferUpload,
			reinterpret_cast<UINT*>(loadedMesh->m_indices.data()), 
			sizeof(UINT), 
			static_cast<UINT>(loadedMesh->m_indices.size()),
			D3D12_RESOURCE_STATE_INDEX_BUFFER
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
