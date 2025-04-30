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
#include <fstream>
void GameEntity::WriteComponentToFile(const Entity entity, std::fstream& file)
{
	GameEntity& gameEntity = REGISTRY->Get<GameEntity>(entity);
	
	file.write((char*)&gameEntity.m_state, sizeof(STATE));
	
	const uint32_t nameStrCount = gameEntity.m_name.size();
	file.write((char*)&nameStrCount, sizeof(uint32_t));
	file.write(&gameEntity.m_name[0], nameStrCount);

	const uint32_t tagStrCount = gameEntity.m_tag.size();
	file.write((char*)&tagStrCount, sizeof(uint32_t));
	file.write(&gameEntity.m_tag[0], tagStrCount);
	
	file.write((char*)&gameEntity.m_layer,  sizeof(uint32_t));
	file.write((char*)&gameEntity.m_static, sizeof(bool));
}
void GameEntity::LoadFileToComponent(const Entity entity, std::fstream& file)
{
	GameEntity& gameEntity = REGISTRY->Get<GameEntity>(entity);

	file.read((char*)&gameEntity.m_state, sizeof(STATE));

	uint32_t nameStrCount = UINT32_MAX;
	file.read((char*)&nameStrCount, sizeof(uint32_t));
	gameEntity.m_name = std::string("", nameStrCount);
	file.read(&gameEntity.m_name[0], nameStrCount);

	uint32_t tagStrCount = UINT32_MAX;
	file.read((char*)&tagStrCount, sizeof(uint32_t));
	gameEntity.m_tag = std::string("", tagStrCount);
	file.read(&gameEntity.m_tag[0], tagStrCount);

	file.read((char*)&gameEntity.m_layer, sizeof(uint32_t));
	file.read((char*)&gameEntity.m_static, sizeof(bool));
}
