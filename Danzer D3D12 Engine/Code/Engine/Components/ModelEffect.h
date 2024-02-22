#pragma once

struct ModelEffect {
	ModelEffect() : m_effectID(0) {}
	ModelEffect(const unsigned int id)
		: m_effectID(id)
	{};

	unsigned int m_effectID;
};