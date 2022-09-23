#pragma once

#include "ModelData.h"
#include "Components/Model.h"
#include "ModelLoaderCustom.h"

class Scene;
class RenderManager;
class TextureHandler;
class DirectX12Framework;

class ModelHandler
{
public:
	ModelHandler() = delete;
	ModelHandler(DirectX12Framework& framework, TextureHandler& textureHandler) :
		m_framework(framework),
		m_textureHandler(textureHandler),
		m_modelLoader()
	{}

	//void Init(DirectX12Framework& framework, RenderManager& renderManager);
	~ModelHandler();

	Model CreateCustomModel(CustomModel model, std::wstring texturePath = L"Sprites/defaultTexture.dds", bool transparent = false);
	Model LoadModel(std::wstring fileName, bool transparent = false, bool uvFlipped = false);
	Model LoadModel(LoaderModel* loadedModel, bool transparent = false, bool uvFlipped = false);
	UINT GetExistingModel(std::string modelName);

	UINT LoadModelFromLevel(LoaderModel* loadedModel, std::vector<UINT>& textures, bool transparent = false, bool uvFlipped = false);

	ModelData& GetLoadedModelInformation(UINT id) {
		if (id - 1 < m_models.size() && id != 0)
			return m_models[id - 1];

		printf("Model with ID:%d doesnt exist! \n", id);
		return m_models[0];
	}

	ModelLoaderCustom& GetModelLoader() { return m_modelLoader; }

	//void SetTexturesForModel(UINT model, std::vector<std::string> textures);
	void SetAlbedoForModel(UINT model,   std::vector<std::string> textures);
	void SetNormalForModel(UINT model,   std::vector<std::string> textures);
	void SetMaterialForModel(UINT model, std::vector<std::string> textures);
	void SetMaterialValues(UINT model, float metallic, float roughness, float emissive);

	std::vector<ModelData>& GetAllModels() {
		return m_models;
	}

private:	
	

	UINT GetNewlyCreatedModelID(ModelData model);

	std::vector<ModelData::Mesh> LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::string name);
	std::vector<ModelData::Mesh> LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::vector<UINT>& textures);
	std::string SetModelName(std::wstring modelPath);

	ModelLoaderCustom m_modelLoader;
	
	std::vector<ModelData> m_models;

	// Used to easily create necessary buffers for Models.
	TextureHandler& m_textureHandler;
	DirectX12Framework& m_framework;
};

