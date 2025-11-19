#pragma once
#include "ComponentRegister.h"
#include <vector>

// Game Entity holds general information of the object. Required component to render and update in scene
struct GameEntity : public BaseComponent {
	COMP_FUNC(GameEntity)

	GameEntity() {}
	explicit GameEntity(entt::entity entity) :
		m_entity(entity)
	{
	}
	~GameEntity() {}

	enum STATE : uint8_t { // Game Entities state, 
		ACTIVE,
		DEACTIVE,
		DESTROY,
	};

	entt::entity m_entity;

	// This stuff needs to be updated
	STATE m_state = STATE::ACTIVE;

	std::string  m_name = "";
	std::string  m_tag = "default";
	unsigned int m_layer = 0;
	bool	     m_static = false;

	void DisplayInEditor(entt::entity entity) override;

	void DettachFromParent() {
		if (m_parent) {
			m_parent->RemoveChildFromEntity(this);
		}
	}
	void AttachToParent(GameEntity* parent) {
		if (parent) {
			if (m_parent) {
				DettachFromParent();
			}

			m_parent = parent;
			m_parent->m_children.push_back(this);
		}
	}

	const std::vector<std::string>& FetchEmplacedComponents() { return m_emplacedComponents; }

private:
#ifdef EDITOR_DEBUG_VIEW
	friend class ImguiHandler;
#endif
	friend class RegistryWrapper;
	friend class Impl;
	friend class SceneLoader;
	
	void WriteComponentToFile(const Entity entity, std::fstream& file) override;
	void LoadFileToComponent(const Entity entity, std::fstream& file) override;
	bool RemoveChildFromEntity(GameEntity* child) {
		for (uint32_t i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] == child) {
				m_children.erase(m_children.begin() + i);
				return true;
			}
		}
		return false;
	}
	
	// Need to figure out how to deal with children....
	std::vector<GameEntity*> m_children;
	GameEntity*				 m_parent = nullptr;

	std::vector<std::string> m_emplacedComponents;
};
REGISTER_COMPONENT(GameEntity);

