#include "stdafx.h"
#include "Components/AllComponents.h"

#include "RegistryWrapper.h"

#include <algorithm>

RegistryWrapper* RegistryWrapper::m_singleton = nullptr;

Entity RegistryWrapper::CreateEmptyEntity()
{
	return m_registry.create();
}

// Creates entity with GameEntity and Transform automatically emplaced. For everything that will be active in the 3D World. 
Entity RegistryWrapper::Create3DEntity(std::string name, bool setStatic)
{
	Entity	    entity   = m_registry.create();
	GameEntity& gameEntt = Emplace<GameEntity>(entity, entity);
	gameEntt.m_state     = GameEntity::STATE::ACTIVE;
	gameEntt.m_static    = setStatic;
	gameEntt.m_name      = name;

	// Default transform on to newly created entity. 
	Emplace<Transform>(entity);
		
	return entity;
}

bool RegistryWrapper::HasComponent(const Entity entity, const std::string componentName)
{
	return ComponentExists(componentName, m_registry.get<GameEntity>(entity));
}

void RegistryWrapper::DestroyEntity(const Entity entity) {
	m_registry.destroy(entity);
}

bool RegistryWrapper::ComponentExists(const std::string componentName, const GameEntity& gameEntity)
{
	const std::vector<std::string>& vec = gameEntity.m_emplacedComponents;
	if (std::find(vec.begin(), vec.end(), componentName) != vec.end()) {
		return true;
	}
	return false;
}

bool RegistryWrapper::RegisterComponentToEntity(const Entity entity, std::string componentName)
{
	GameEntity* gameEntity = m_registry.try_get<GameEntity>(entity);
	assert(gameEntity, "WARNING! GameEntity component has not been emplaced on Entity...");

	std::vector<std::string>& vec = gameEntity->m_emplacedComponents;

	componentName.erase(componentName.begin(), componentName.begin() + componentName.find_first_of(" ") + 1);

	// Check first so that this entity does not already have this component.
	if (!ComponentExists(componentName, *gameEntity)) {
		gameEntity->m_emplacedComponents.emplace_back(componentName);
		return true;
	}

	return false;
}
