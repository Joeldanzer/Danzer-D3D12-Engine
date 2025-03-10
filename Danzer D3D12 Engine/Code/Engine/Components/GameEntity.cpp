#include "stdafx.h"
#include "GameEntity.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
const int MaxNameSize = 50;
void GameEntity::DisplayInEditor(entt::entity entity)
{
	GameEntity& gameEntity = REGISTRY->Get<GameEntity>(entity);

	char name[MaxNameSize];
	strncpy_s(name, MaxNameSize, gameEntity.m_name.c_str(), gameEntity.m_name.size());
	
	ImGui::InputText("Name", name, MaxNameSize);
	gameEntity.m_name = name;

	bool isStatic = gameEntity.m_static;
	ImGui::Selectable("Static", isStatic);
	gameEntity.m_static = isStatic;

	const std::string states[3] = { "Active", "Deactive", "Destroy" };
	if (ImGui::BeginCombo("State", states[gameEntity.m_state].c_str())) {
		for (uint32_t i = 0; i < 3; i++)
		{
			bool selected = false;
			ImGui::Selectable(states[i].c_str(), &selected);
			if (selected)
				gameEntity.m_state = (GameEntity::STATE)i;

		}
		ImGui::EndCombo();
	}
}
#else
void GameEntity::DisplayInEditor(entt::entity entity) {}
#endif