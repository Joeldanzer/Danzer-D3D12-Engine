#pragma once
#include "../VertexAndTextures.h"
#include "Rendering/Buffers/VertexBuffer.h"
#include "Rendering/Buffers/MaterialBuffer.h"
#include "Rendering/FrustrumCulling.h"

#include "Material.h"

#include <array>
#include <string>

//Used to make simple custom models like triangles or cubes
struct CustomModel {
	std::vector<Vertex>		  m_verticies;
	std::vector<unsigned int> m_indices;
	std::string				  m_customModelName = "customModel";
};

class ModelData
{
public:
	static CustomModel GetCube();
	static CustomModel GetPlane();

	struct Mesh {
		uint32_t				 m_vertexSize;
		uint32_t				 m_numIndices;
		uint32_t				 m_numVerticies;
		
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW	 m_indexBufferView  = {};
		
		ComPtr<ID3D12Resource>	 m_indexBuffer;
		ComPtr<ID3D12Resource>	 m_vertexBuffer;
		
		Material				 m_material;
		MaterialBuffer			 m_materialBuffer;
	
		FrustrumCulling::AABB	 m_aabb;

		VertexBuffer			 m_meshBuffer;
		std::vector<Mat4f>		 m_instanceTransforms;

		bool m_renderMesh = true;
	};


	ModelData() = delete;
	~ModelData() = default;
	explicit ModelData(const ModelData&) = default;
	explicit ModelData(std::vector<Mesh>& data, ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, std::vector<Vect3f>& verticies, std::wstring fileName, std::string name = "", bool transparent = false) :
		m_ID(0),
		m_name(name),
		m_meshes(data),
		m_renderModel(false),
		m_verticies(verticies),
		m_transparent(transparent),
		m_modelPath(fileName)
	{
		for (auto& mesh : m_meshes) {
			mesh.m_materialBuffer.Init(device, cbvWrapper, sizeof(MaterialBuffer::Data));
			mesh.m_meshBuffer.Initialize(device, sizeof(Mat4f));
		}

		m_transformBuffer.Initialize(device, sizeof(Mat4f));
	};
	explicit ModelData(const uint32_t id, const bool transparent, const std::wstring modelPath, const std::string name) : 
		m_finishedLoading(false),
		m_renderModel(false),
		m_ID(id),
		m_transparent(transparent),
		m_modelPath(modelPath),
		m_name(name)
	{}

	void ClearInstanceTransform();
	const std::vector<Mat4f>& GetInstanceTransforms() { return m_instanceTransforms; }
	const std::vector<uint32_t>& MeshToRender() { return m_meshToRender; }

	void SetID(UINT id) { m_ID = id; }
	const UINT GetID() { return m_ID; }

	const bool& IsTransparent() { return m_transparent; }

	void SetName(std::string name) { m_name = name; }
	const std::string& Name() const { return m_name; }

	VertexBuffer& GetTransformInstanceBuffer() {
		return m_transformBuffer;
	}
	void AddModelInstanceTransform(Mat4f transform) { m_instanceTransforms.emplace_back(transform); }
	void AddMeshToRender(const uint32_t meshIndex, const Mat4f& transform) {
		m_meshes[meshIndex].m_instanceTransforms.emplace_back(transform.Transpose());
		m_meshToRender.emplace_back(meshIndex);
	}
	void UpdateAllMeshInstanceBuffer(uint32_t frameIndex);
	void UpdateTransformInstanceBuffer(uint32_t frameIndex) {
		m_transformBuffer.UpdateBuffer(reinterpret_cast<uint16_t*>(&m_instanceTransforms[0]), (uint32_t)m_instanceTransforms.size(), frameIndex);
	}

	void UpdatedMaterialBuffer(UINT frameIndex) {
		//MaterialBuffer::Data data;
		//data.m_hasMaterialTexture = m_hasMaterialTextures;
		//if (!m_hasMaterialTextures) {
		//	data.m_emissive =  m_material.m_emissive;
		//	data.m_roughness = m_material.m_roughnes;
		//	data.m_metallic =  m_material.m_metallic;
		//}
		//
		//m_materialBuffer.UpdateBuffer(frameIndex, reinterpret_cast<UINT8*>(&data));
	}
	std::vector<Mesh>& GetMeshes() {return m_meshes;}
	Mesh& GetSingleMesh(UINT index) { return m_meshes[index]; }

	std::vector<Vect3f>& GetModelPositionVerticies() {
		return m_verticies;
	}

	std::vector<UINT>& GetAlbedoTextures() {
		return m_albedoTextures;
	}
	std::vector<UINT>& GetNormalTextures() {
		return m_normalTextures;
	}
	std::vector<UINT>& GetMaterialTextures() {
		return m_materialTextures;
	}

	std::wstring& GetModelPath() {
		return m_modelPath;
	}

	const bool ModelFinished() {
		return m_finishedLoading;
	}

private:
	friend class ModelHandler;

	void FinilizeModelData(std::vector<Mesh>& data, ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, std::vector<Vect3f>& verticies) {
		m_meshes    = data;
		m_verticies = verticies;

		for (auto& mesh : m_meshes) {
			mesh.m_materialBuffer.Init(device, cbvWrapper, sizeof(MaterialBuffer::Data));
			mesh.m_meshBuffer.Initialize(device, sizeof(Mat4f));
		}

		m_transformBuffer.Initialize(device, sizeof(Mat4f));

		m_finishedLoading = true;
	}

	std::vector<Mesh>   m_meshes;
	std::vector<Vect3f> m_verticies;

	//std::vector<>
	std::vector<uint32_t> m_meshToRender;

	std::vector<Mat4f> m_instanceTransforms;
	std::vector<Mat4f> m_instanceTransparentTransforms;

	// Vertext paint inforamtion
	std::vector<uint32_t> m_albedoTextures;
	std::vector<uint32_t> m_normalTextures;
	std::vector<uint32_t> m_materialTextures;

	VertexBuffer m_transformBuffer;
	VertexBuffer m_transparentTransformBuffer;

	uint32_t m_ID;	

	std::string  m_name;
	std::wstring m_modelPath;

	bool m_renderModel;
	bool m_transparent;
	
	bool m_finishedLoading;

};
