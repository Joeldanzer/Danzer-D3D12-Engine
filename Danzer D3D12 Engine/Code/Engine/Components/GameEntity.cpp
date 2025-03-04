#include "stdafx.h"
#include "GameEntity.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void GameEntity::DisplayInEditor(entt::entity entity)
{
	GameEntity& gameEntity = REGISTRY->Get<GameEntity>(entity);

	//static char* name = new char;
	//int length = (int)strlen(gameEntity.m_name.c_str());
	//memcpy(name, gameEntity.m_name.c_str(), length + 1);
	//
	//ImGui::InputText("Entity Name", name, 32);
	//gameEntity.m_name = name;

	bool isStatic = gameEntity.m_static;
	ImGui::Selectable("Static", isStatic);
	gameEntity.m_static = isStatic;

	const std::string states[3] = { "Active", "Deactive", "Destroy" };
	if (ImGui::BeginCombo("Entity State", states[gameEntity.m_state].c_str())) {
		for (uint32_t i = 0; i < 3; i++)
		{
			bool selected = false;
			ImGui::Selectable(states[i].c_str(), selected);
			if (selected)
				gameEntity.m_state = (GameEntity::STATE)i;

		}
		ImGui::EndCombo();
	}
}
#else
void GameEntity::DisplayInEditor(entt::entity entity) {}
#endif