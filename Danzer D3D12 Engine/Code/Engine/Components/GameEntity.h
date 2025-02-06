#pragma once
#include "entt/entt.hpp"
#include "ComponentRegister.h"

// Game Entity holds general information of the object. Required component to render and update in scene
struct GameEntity : public BaseComponent {
	explicit GameEntity(entt::entity entity) : 
		m_entity(entity)
	{}
	~GameEntity() {}
	
	enum STATE : uint8_t { // Game Entities state, 
		ACTIVE,
		DEACTIVE,
		DESTROY,
	};
	
	entt::entity m_entity;

	// This stuff needs to be updated
	STATE m_state		  = STATE::ACTIVE;
 
	std::string  m_name   = "";
	std::string  m_tag    = "default";
	unsigned int m_layer  = 0;
	bool	     m_static = false;

	// For editor if i'll decide to keep working on it.
	bool  m_selected = false; 
}; 