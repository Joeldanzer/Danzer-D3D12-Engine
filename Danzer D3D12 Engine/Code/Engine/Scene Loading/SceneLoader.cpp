#include "stdafx.h"
#include "SceneLoader.h"

#include <filesystem>
#include <fstream>

#include "SceneManager.h"
#include "Components/AllComponents.h"

const std::wstring SceneLoader::SceneFileType = L".bsf";

void SceneLoader::SaveScene(const std::wstring fileName)
{
	RLH::Instance().QueueLoadRequest(new SceneRequest(*this, SceneRequest::RequestType::SAVE, fileName));
}
void SceneLoader::SaveScene(const std::wstring fileLocation, const std::wstring nameOfScene)
{
	const std::wstring fileName = fileLocation + L"\\" + nameOfScene + SceneFileType;
	RLH::Instance().QueueLoadRequest(new SceneRequest(*this, SceneRequest::RequestType::SAVE, fileName));
}

void SceneLoader::LoadScene(const std::wstring fileName)
{
	RLH::Instance().QueueLoadRequest(new SceneRequest(*this, SceneRequest::RequestType::LOAD, fileName));
}

void SceneLoader::WriteSceneToFile(const std::wstring fileName)
{
	std::string testName = "TestScene.bsf";
	std::fstream sceneFile(testName, std::ios_base::out | std::ios_base::trunc);

	auto scene = REGISTRY->GetRegistry().view<GameEntity, Transform>();
	std::vector<Entity> entities;

	for (const Entity entity : scene)
		entities.emplace_back(entity);

	// Wrtie number of entities in the scene.
	uint32_t entityCount = (uint32_t)entities.size();
	sceneFile.write((char*)&entityCount, sizeof(uint32_t));
	for (uint32_t i = 0; i < entities.size(); i++)
	{
		GameEntity& gameEntity = REGISTRY->Get<GameEntity>(entities[i]);
		const std::vector<std::string>& components = gameEntity.FetchEmplacedComponents();

		const uint32_t componentCount = (uint32_t)components.size();
		sceneFile.write((char*)&componentCount, sizeof(uint32_t));
		for (uint32_t j = 0; j < components.size(); j++)
		{
			const std::string& componentName = components[j];
			BaseComponent* base = COMPONENT_ENTRY_REGISTER.GetBaseComponent(entities[i], componentName);

			if (!base)
				continue;

			const uint32_t textLength = static_cast<uint32_t>(componentName.size());

			//write size of string and name of component to file.
			sceneFile.write((char*)&textLength, sizeof(uint32_t));
			sceneFile.write(&componentName[0], textLength);

			// Write all the component data from the char* to the file. 
			base->WriteComponentToFile(entities[i], sceneFile);
		}
	}

	// Delete the data after writing to file.
	sceneFile.close();
}

bool SceneLoader::ReadSceneFromFile(const std::wstring fileName)
{
	if (!std::filesystem::exists(fileName))
		return false;

	std::fstream sceneFile(fileName, std::ios_base::in);

	m_sceneIsLoading = true;
	// Clears whole scene but doesn't clear any rendering data stored on the cpu and gpu.
	REGISTRY->ClearWholeScene();
	
	//* Component Data reading structure:
	//*  
	//* - Entity Count
	//* for	each (entity)
	//*		- Component Count
	//*		for each (component)
	//*			- char count in name
	//*			- Component Name
	//*
	//*			- Load the component data from file to component
	//*
	//* EoF

	uint32_t entityCount = 0;
	sceneFile.read((char*)&entityCount, sizeof(uint32_t));
	for (uint32_t i = 0; i < entityCount; i++)
	{
		Entity entity = REGISTRY->CreateEmptyEntity();

		uint32_t componentCount = 0;
		sceneFile.read((char*)&componentCount, sizeof(uint32_t));
		for (uint32_t j = 0; j < componentCount; j++)
		{
			uint32_t strSize = 0;
			sceneFile.read((char*)&strSize,   sizeof(uint32_t));
			std::string componentName("", strSize);
			sceneFile.read(&componentName[0], strSize);

			COMPONENT_ENTRY_REGISTER.EmplaceComponent(entity, componentName);
			BaseComponent* base = COMPONENT_ENTRY_REGISTER.GetBaseComponent(entity, componentName); 
			if (base)
				base->LoadFileToComponent(entity, sceneFile);
			else
				std::cout << "Failed to load component: " << componentName << std::endl;

			if (componentName == "Camera")
				Engine::Instance().GetSceneManager().SetMainCamera(entity);
		}
	}

	m_sceneIsLoading = false;

	return true;
}

void SceneLoader::SceneRequest::LoadData()
{
	switch (m_type) {
	case LOAD:
		m_sceneLoader.ReadSceneFromFile(m_fileName);
		break;
	case SAVE:
		m_sceneLoader.WriteSceneToFile(m_fileName);
		break;
	}
}

