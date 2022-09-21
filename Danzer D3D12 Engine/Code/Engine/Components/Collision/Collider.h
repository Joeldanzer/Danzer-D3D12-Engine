#pragma once
#include "Core/MathDefinitions.h"

#include <algorithm>
#include <vector>
#include <string>

#include "entt/entt.hpp"

class Collider {
public:
	Vect3f m_offset;
	
	bool IntersectedWith(std::string tag) {
		for (unsigned int i = 0; i < m_intersectList.size(); i++)
		{
			if (tag == m_intersectList[i].first) {
				return true;
			}
		}
		return false;
	}
protected:
	friend class CollisionManager;

	Vect3f m_position;
	std::vector<std::pair<std::string, entt::entity>> m_intersectList;
};