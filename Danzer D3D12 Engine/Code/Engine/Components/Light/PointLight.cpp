#include "stdafx.h"
#include "PointLight.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void PointLight::DisplayInEditor(Entity entity)
{
	PointLight& point = REGISTRY->Get<PointLight>(entity);

	float color[3] = { point.m_color.x, point.m_color.y, point.m_color.z };
	ImGui::ColorPicker3("Light Color", &color[0]);

	float strength = point.m_color.w;
	ImGui::DragFloat("Light Strength", &strength, 0.01f, 0.0f, 10.0f);

	float offset[3] = { point.m_offset.x, point.m_offset.y, point.m_offset.z };
	ImGui::DragFloat3("Offset Position", &offset[0]);

	float radius = point.m_range;
	ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f);

	point.m_color  = { color[0], color[1], color[2], strength };
	point.m_offset = { offset[0], offset[1], offset[2] };
	point.m_range  = radius;
}
#else
void PointLight::DisplayInEditor(Entity entity){}
#endif
