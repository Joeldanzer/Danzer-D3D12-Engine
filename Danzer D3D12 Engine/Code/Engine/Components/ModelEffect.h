#pragma once

struct ModelEffect {
	ModelEffect(const unsigned int id)
		: m_effectID(id)
	{};

	const unsigned int m_effectID;
};