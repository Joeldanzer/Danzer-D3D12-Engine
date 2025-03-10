#pragma once
#include "entt/entt.hpp"

#define COMPONENT_NAME(ComponentName) \
    std::string(#ComponentName)

struct GameEntity;

typedef entt::entity Entity;
// Wrapper for entt::registry, this wrapper is designed to keep track what components entities have on them. 
// Making it easier to fetch the data you want and to make working with the editor much smoother. 
class RegistryWrapper
{
public:
	void operator=(RegistryWrapper& other) = delete; // We don't want to be able to copy an instance. 

	static RegistryWrapper* Instance() {
		if (m_singleton == nullptr)
			m_singleton = new RegistryWrapper();
		return m_singleton;
	}

	Entity CreateEmptyEntity();
	Entity Create3DEntity(std::string name, bool setStatic = false);

	template<typename Component, typename ... Args>
	Component& Emplace(const Entity entity, Args&& ...args);

	template<typename Component>
	Component& Get(const Entity entity);

	template<typename Component>
	Component* TryGet(const Entity entity);
	
	template<typename Component>
	bool HasComponent(const Entity entity);

	bool HasComponent(const Entity entity, const std::string componentName);

	Entity GetEntityWithName(const std::string name);
	bool   EntityWithNameExists(const std::string name);

	template<typename Component>
	const std::string FetchComponentName();

	void DestroyEntity(const Entity entity);

	entt::registry& GetRegistry() {
		return m_registry;
	}

private:
	RegistryWrapper(){}
	~RegistryWrapper(){}

	bool ComponentExists(const std::string componentName, const GameEntity& gameEntity);
	bool RegisterComponentToEntity(const Entity entity, std::string componentName);

	static RegistryWrapper* m_singleton;

	entt::registry m_registry;
};

typedef RegistryWrapper Reg;

#define REGISTRY RegistryWrapper::Instance()

template<typename Component, typename ... Args>
inline Component& RegistryWrapper::Emplace(const Entity entity, Args&& ... args)
{
	Component& component = m_registry.emplace<Component>(entity, args ...);
	RegisterComponentToEntity(entity, typeid(Component).name());

	return component;
}

template<typename Component>
inline Component& RegistryWrapper::Get(const Entity entity)
{
	return m_registry.get<Component>(entity);
}

template<typename Component>
inline Component* RegistryWrapper::TryGet(const Entity entity)
{
	return m_registry.try_get<Component>(entity);
}

template<typename Component>
inline bool RegistryWrapper::HasComponent(const Entity entity)
{
	if (ComponentExists(FetchComponentName<Component>(), m_registry.get<Component>())) {
		return true;
	}
	return false;
}

template<typename Component>
inline const std::string RegistryWrapper::FetchComponentName()
{
	std::string componentName = typeid(Component).name();
	return componentName.erase(componentName.begin(), componentName.begin() + componentName.find_first_of(" ") + 1);
}
