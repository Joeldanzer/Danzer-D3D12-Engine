#pragma once
#include "entt/entt.hpp"

#define COMPONENT_NAME(ComponentName) \
    std::string(#ComponentName)

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

	void DestroyEntity(const Entity entity);

	entt::registry& GetRegistry() {
		return m_registry;
	}

private:
	RegistryWrapper(){}
	~RegistryWrapper(){}

	bool RegisterComponentToEntity(const Entity entity, std::string componentName);

	static RegistryWrapper* m_singleton;

	entt::registry m_registry;
};

typedef RegistryWrapper Reg;

#undef  REGISTRY
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