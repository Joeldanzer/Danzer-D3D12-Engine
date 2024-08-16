#pragma once
#include <string>

// Game Entity holds general information of the object. Required component to render and update in scene
struct GameEntity{
	GameEntity(){}

	enum class STATE { // Game Entities state, 
		ACTIVE,
		NOT_ACTIVE,
		DESTROY,
	};

	STATE m_state  = STATE::ACTIVE;
	std::string  m_name  = "";
	std::string  m_tag   = "default";
	unsigned int m_layer = 0;

	
	bool  m_static = false;

	// For editor if i'll decide to keep working on it.
	bool  m_selected = false;
}; 