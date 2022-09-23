#pragma once
#include "TransformBuffer.h"
#include "../VertexAndTextures.h"
#include "Rendering/Buffers/MaterialBuffer.h"

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
	
	struct Material {
		float m_roughnes = 0.1f;
		float m_metallic = 0.1f;
		float m_emissive = 0.0f;
	};

	struct Mesh {
		UINT m_numVerticies;
		UINT m_vertexSize;

		UINT m_numIndices;

		D3D12_VERTEX_BUFFER_VIEW	 m_vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW		 m_indexBufferView = {};

		ComPtr<ID3D12Resource>		 m_indexBuffer;
		ComPtr<ID3D12Resource>		 m_vertexBuffer;
		
		bool m_renderMesh = true;
	};


	ModelData() = delete;
	~ModelData() = default;
	explicit ModelData(const ModelData&) = default;
	explicit ModelData(std::vector<Mesh>& data, ID3D12Device* device, std::vector<Vect3f>& verticies, std::string name = "", bool transparent = false) :
		m_ID(0),
		m_name(name),
		m_meshes(data),
		m_renderModel(false),
		m_verticies(verticies),
		m_transparent(transparent)
	{
		m_transformBuffer.Init(device);
		m_materialBuffer.Init(device);
	};

	void AddInstanceTransform(Mat4f transform);
	void ClearInstanceTransform();
	const std::vector<Mat4f>& GetInstanceTransforms() { return m_instanceTransforms; }

	void SetID(UINT id) { m_ID = id; }
	const UINT GetID() { return m_ID; }

	const bool& IsTransparent() { return m_transparent; }

	void SetName(std::string name) { m_name = name; }
	const std::string& Name() const { return m_name; }

	MaterialBuffer& GetMaterialBuffer() {
		return m_materialBuffer;
	}
	TransformBuffer& GetTransformInstanceBuffer() {
		return m_transformBuffer;
	}
	void UpdateTransformInstanceBuffer(UINT frameIndex) { 
		m_transformBuffer.UpdateBuffer(reinterpret_cast<UINT8*>(&m_instanceTransforms[0]), (UINT)m_instanceTransforms.size(), frameIndex); 
	}
	void UpdatedMaterialBuffer(UINT frameIndex) {
		MaterialBuffer::Data data;
		data.m_hasMaterialTexture = m_hasMaterialTextures;
		if (!m_hasMaterialTextures) {
			data.m_emissive =  m_material.m_emissive;
			data.m_roughness = m_material.m_roughnes;
			data.m_metallic =  m_material.m_metallic;
		}

		m_materialBuffer.UpdateBuffer(frameIndex, reinterpret_cast<UINT8*>(&data));
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

	Material& GetMaterialData() {
		return m_material;
	}
	bool& HasMaterialTextures() {
		return m_hasMaterialTextures;
	}

private:
	friend class ModelHandler;

	std::vector<Mesh>  m_meshes;
	std::vector<Vect3f> m_verticies;
	
	std::vector<Mat4f> m_instanceTransforms;
	std::vector<Mat4f> m_instanceTransparentTransforms;

	//std::vector<std::array<UINT, 3>> m_textures;
	std::vector<UINT> m_albedoTextures;
	std::vector<UINT> m_normalTextures;
	std::vector<UINT> m_materialTextures;

	TransformBuffer m_transformBuffer;
	TransformBuffer m_transparentTransformBuffer;
	MaterialBuffer m_materialBuffer;

	UINT m_ID;	

	std::string m_name;

	Material m_material;
	bool m_hasMaterialTextures = false;

	bool m_renderModel;
	bool m_transparent;
};
