#pragma once

#include "..\..\Core\MathDefinitions.h"

#include "assimp/Importer.hpp"

#include <unordered_map>
#include <vector>
#include <memory>

struct aiMesh;
struct aiNode;
struct aiScene;
struct aiMaterial;

struct LoaderMesh {
	LoaderMesh() :
		m_indices(),
		m_shaderType(0),
		m_vertexSize(0),
		m_vertexCount(0),
		m_textureIndex(0),
		m_verticies(nullptr),
		m_name("##")
	{}
	~LoaderMesh() {
		delete m_verticies;
	}

	Mat4f m_meshTransform;
	UINT  m_textureIndex;
	std::string m_name;
	std::vector<uint32_t> m_indices;
	char* m_verticies;

	uint32_t m_vertexCount;
	uint32_t m_vertexSize;
	uint32_t m_shaderType;
	
	Vect3f m_aabbMin;
	Vect3f m_aabbMax;

	std::vector<std::wstring> m_textures;
};

struct LoaderModel {
	LoaderModel() : 
		m_scene(nullptr),
		m_meshes(),
		m_inverseMatrix(Mat4f()),
		m_isLoaded(false),
		m_isTransparent(false)
	{}
	~LoaderModel() { for (auto* m : m_meshes) { delete m; } }

	std::string m_name;
	std::vector<LoaderMesh*> m_meshes;
	const aiScene* m_scene;
	std::vector<Vect3f> m_verticies;
	std::vector<std::string> m_textures;
	Mat4f m_inverseMatrix;
	bool m_isLoaded;
	bool m_isTransparent;
};

// Collect Vertex Info from Assimp aiMesh
struct VertexCollector {
	void PushVec4(Vect4f vertex) {
		m_vertexInfo.emplace_back(vertex.x);
		m_vertexInfo.emplace_back(vertex.y);
		m_vertexInfo.emplace_back(vertex.z);
		m_vertexInfo.emplace_back(vertex.w);
	}

	void PushVec2(Vect2f vertex) {
		m_vertexInfo.emplace_back(vertex.x);
		m_vertexInfo.emplace_back(vertex.y);
	}

	std::vector<float> m_vertexInfo;
};


class ModelLoaderCustom
{
public:
	enum LoaderTypes {
		LoaderType_Position,
		LoaderType_UV,
		LoaderType_Normal,
		LoaderType_BinormTan,
		LoaderType_Bones,
		LoaderType_Color,
	};

	ModelLoaderCustom();
	~ModelLoaderCustom();

	bool CheckModelMemory(const aiScene* scene);

	std::unique_ptr<LoaderModel> LoadModelFromAssimp(std::string fileName, bool uvFlipped);
	std::vector<std::unique_ptr<LoaderModel>> LoadMultipleModelsFromAssimp(std::string fileName, std::vector<std::pair<std::string, Mat4f>>& transforms, bool uvFlipped);
	std::unique_ptr<LoaderModel> LoadModelFromAiNode(const aiScene* scene, aiNode* node, std::vector<UINT>& textures, bool uvFlipped);
	Mat4f ConvertToEngineMat4(const aiMatrix4x4& assimpMatrix);
	std::vector<std::string> GetAllTexturesFromScene(const aiScene* scene);
	std::string FixModelName(std::string name);

private:
	void FetchAllModelsInScene(const aiScene* scene, std::vector<std::unique_ptr<LoaderModel>>& models, std::vector<std::pair<std::string, Mat4f>>& transforms, bool uvFlipped);
	void GetAllModelProperties(LoaderModel* out, aiNode* child, const aiScene* scene, Mat4f parentMat, bool uvFlipped);
	void LoadVerticies(std::vector<Vect3f>& v3Verts, aiMesh* mesh, LoaderMesh* loaderMesh, bool uvFlipped);
	void LoadVerticiesWithTransform(std::vector<Vect3f>& v3Verts, aiMesh* mesh, LoaderMesh* loaderMesh, Mat4f transform, bool uvFlipped);
	void LoadMaterials(const aiScene* scene, LoaderModel* model);
	void LoadTexture(int type, int textureIndex, std::vector<std::string>& textures, aiMaterial* material);
	void LoadTexture(int type, std::vector<std::string>& textures, aiMaterial* material, const aiScene* scene);
	void FetchMeshOfChild(const aiScene* scene, aiNode* child, const Mat4f& transform, LoaderModel* out, bool uvFlipped);

	bool CheckModelName(std::vector<std::unique_ptr<LoaderModel>>& models, std::string& name);

	Assimp::Importer m_importer;
};

