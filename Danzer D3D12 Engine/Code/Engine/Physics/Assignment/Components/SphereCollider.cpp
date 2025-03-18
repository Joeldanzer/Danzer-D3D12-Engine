#include "stdafx.h"
#include "SphereCollider.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void SphereCollider::DisplayInEditor(const Entity entity)
{
	SphereCollider& sphere = REGISTRY->Get<SphereCollider>(entity);
	ImGui::Checkbox("Use Gravity", &sphere.m_gravity);
	ImGui::Checkbox("Kinematic",   &sphere.m_kinematic);
}
#else
void SphereCollider::DisplayInEditor(const Entity entity){}
#endif
