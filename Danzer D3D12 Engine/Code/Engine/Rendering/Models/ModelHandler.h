#pragma once

#include "Material.h"
#include "ModelData.h"
#include "Components/Model.h"
#include "ModelLoaderCustom.h"

#include "entt/entt.hpp"
class Scene;
class RenderManager;
class TextureHandler;
class D3D12Framework;

class ModelHandler
{
public:
	ModelHandler() = delete;
	ModelHandler(D3D12Framework& framework, TextureHandler& textureHandler) :
		m_framework(framework),
		m_textureHandler(textureHandler),
		m_modelLoader()
	{}
	~ModelHandler();

	Model CreateCustomModel(CustomModel model, bool transparent = false);
	Model LoadModel(std::wstring fileName, std::string name = "", bool transparent = false, bool uvFlipped = false);
	void  LoadModelsToScene(entt::registry& reg, std::wstring fileName, std::string name = "", bool uvFlipped = false);
	UINT GetExistingModel(std::wstring modelName);

	ModelData& GetLoadedModelInformation(UINT id) {
		if (id - 1 < m_models.size() && id != 0)
			return m_models[id - 1];

		printf("Model with ID:%d doesnt exist! \n", id);
		return m_models[0];
	}

	ModelLoaderCustom& GetModelLoader() { return m_modelLoader; }

	void SetMaterialForModel(UINT model, Material material, UINT mesh = 0);

	std::vector<ModelData>& GetAllModels() {
		return m_models;
	}

private:	
	Material GetNewMaterialFromLoadedModel(const std::string& material);
	UINT GetNewlyCreatedModelID(ModelData model);

	std::vector<ModelData::Mesh> LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::string name);
	std::vector<ModelData::Mesh> LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::vector<UINT>& textures);
	std::string SetModelName(std::wstring modelPath);

	std::vector<ModelData> m_models;

	// Used to easily create necessary buffers for Models.
	ModelLoaderCustom m_modelLoader;	
	TextureHandler&   m_textureHandler;
	D3D12Framework&   m_framework;
};

