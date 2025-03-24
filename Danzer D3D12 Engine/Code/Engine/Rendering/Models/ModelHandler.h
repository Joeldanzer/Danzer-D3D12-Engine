#pragma once

#include "Material.h"
#include "ModelData.h"
#include "Components/Model.h"
#include "ModelLoaderCustom.h"
#include "entt/entt.hpp"
#include "Core/ResourceLoadingHandler.h"

//#include "../3rdParty/entt/entt.hpp"

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
	Model LoadModel(std::wstring fileName, bool transparent = false, bool uvFlipped = false);
	void  LoadModelsToScene(entt::registry& reg, std::wstring fileName, std::string name = "", bool uvFlipped = false);

	ModelData& GetLoadedModelInformation(UINT id) {
		if (id < m_models.size() && id != UINT32_MAX)
			return m_models[id];

		printf("Model with ID:%d doesnt exist! \n", id);
		return m_models[0];
	}

	ModelLoaderCustom& GetModelLoader() { return m_modelLoader; }
	void SetMaterialForModel(UINT model, Material material, UINT mesh = 0);

	std::vector<ModelData>& GetAllModels() {
		return m_models;
	}

	void LoadRequestedModels();

	const uint32_t ModelExists(const std::wstring modelPath);
	const uint32_t ModelExists(const std::string  name);

private:	
	friend struct ModelLoadRequest;

	struct ModelLoadRequest : public LoadRequest {
		ModelLoadRequest(ModelHandler* modelHandler, const std::wstring fileName, const std::string modelName, bool transparent, bool uvFlipped) :
			m_modelHandler(modelHandler),
			m_fileName(fileName),
			m_modelName(modelName),
			m_transparent(transparent),
			m_uvFlipped(uvFlipped)
		{}

		void LoadData() override;

		ModelHandler* m_modelHandler;
		std::wstring m_fileName;
		std::string  m_modelName;
		bool m_transparent;
		bool m_uvFlipped;
	};

	struct LoadRequestData {
		std::unique_ptr<LoaderModel> m_model;
		bool m_transparent;
		std::string  m_modelName;
		std::wstring m_fileName;
	};

	Material GetNewMaterialFromLoadedModel(const std::string& material);
	UINT GetNewlyCreatedModelID(ModelData model);

	uint32_t CreateModelFromLoadedData(LoaderModel* loadedModel, const std::string name, bool transparent);

	std::vector<ModelData::Mesh> LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::string name);
	std::vector<ModelData::Mesh> LoadMeshFromLoaderModel(LoaderModel* loadedModel, std::vector<UINT>& textures);
	std::string SetModelName(std::wstring modelPath);
	
	std::unique_ptr<LoaderModel> FetchLoaderModel(std::wstring fileName, bool uvFlipped);

	void WriteToBinaryModelFile(const LoaderModel* loadedModel, const std::string modelName, const std::vector<ModelData::Mesh>& meshes);
	void ReadFromBinaryModelFile(std::string modelName, LoaderModel* loaderModel);
	bool BinaryModelFileExists(std::string modelName);

	uint32_t					 m_modelRequestCounter = 1;
	std::vector<LoadRequestData> m_loadRequests;
	std::vector<ModelData>       m_models;

	// Used to easily create necessary buffers for Models.
	ModelLoaderCustom m_modelLoader;	
	TextureHandler&   m_textureHandler;
	D3D12Framework&   m_framework;
};

