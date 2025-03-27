#include "stdafx.h"
#include "BoxCollider.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void BoxCollider::DisplayInEditor(const Entity entity)
{
	BoxCollider& box = REGISTRY->Get<BoxCollider>(entity);	
	ImGui::Checkbox("Use Gravity", &box.m_gravity);
	ImGui::Checkbox("Kinematic",   &box.m_kinematic);
}
#else
void BoxCollider::DisplayInEditor(const Entity entity){}
#endif
