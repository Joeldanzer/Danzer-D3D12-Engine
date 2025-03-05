#include "stdafx.h"
#include "Spotlight.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void SpotLight::DisplayInEditor(const Entity entity)
{
	SpotLight& light = REGISTRY->Get<SpotLight>(entity);

	float color[3] = { light.m_color.x, light.m_color.y, light.m_color.z };
	ImGui::ColorPicker3("Light Color", &color[0]);

	float strength = light.m_color.w;
	ImGui::DragFloat("Light Strength", &strength, 0.01f, 0.0f, 10.0f);

	float offset[3] = { light.m_offset.x, light.m_offset.y, light.m_offset.z };
	ImGui::DragFloat3("Offset Position", &offset[0]);

	float cutOff = light.m_cutOff;
	ImGui::DragFloat("Cut Off", &cutOff, 0.01f, 0.0f, 1.0f);

	float outerCutOff = light.m_outerCutOff;
	ImGui::DragFloat("Outer Cut Off", &outerCutOff, 0.01f, 0.0f, 1.0f);

	float range = light.m_range;
	ImGui::DragFloat("Range", &range, 0.01f, 0.01f, 100.0f);

	light.m_color		= { color[0], color[1], color[2], strength };
	light.m_offset		= { offset[0], offset[1], offset[2] };
	light.m_cutOff		= cutOff;
	light.m_outerCutOff = outerCutOff;
	light.m_range		= range;
}
#else
void SpotLight::DisplayInEditor(const Entity entity){}
#endif