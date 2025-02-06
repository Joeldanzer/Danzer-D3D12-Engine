#pragma once
#include "Core/MathDefinitions.h"
#include <string>

struct TextOLD {
	TextOLD(){}
	TextOLD(std::string text, Vect4f color, unsigned int font) :
		m_text(text),
		m_color(color),
		m_fontID(font)
	{}

	std::string m_text = "";
	unsigned int m_fontID = 0;
	Vect4f m_color = { 1.f, 1.f, 1.f, 1.f };
};
