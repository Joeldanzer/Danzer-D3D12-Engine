#pragma once
#include <string>

struct Object{
	Object(){}

	enum class STATE {
		ACTIVE,
		NOT_ACTIVE,
		DESTROY,
	};

	std::string  m_name  = "";
	std::string  m_tag   = "default";
	unsigned int m_layer = 0;

	STATE m_state  = STATE::ACTIVE;
	bool  m_static = false;
}; 