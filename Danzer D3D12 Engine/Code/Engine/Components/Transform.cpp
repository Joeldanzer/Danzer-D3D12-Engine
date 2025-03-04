#include "stdafx.h"
#include "Transform.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void Transform::DisplayInEditor(Entity entity)
{
	Transform& transform = REGISTRY->Get<Transform>(entity);

	float position[3] = {transform.m_position.x, transform.m_position.y, transform.m_position.z };
	float scale[3]	  = {transform.m_scale.x,    transform.m_scale.y,    transform.m_scale.z	};
	
	Vect3f euler = transform.m_rotation.ToEuler();
	float rotation[3] = { ToDegrees(euler.x), ToDegrees(euler.y), ToDegrees(euler.z) };
	
	ImGui::DragFloat3("Position", position, 0.01f, -FLT_MAX, FLT_MAX);
	ImGui::DragFloat3("Rotation", rotation, 0.1f,  -360.f, 360.f);
	ImGui::DragFloat3("Scale",    scale,    0.01f, -FLT_MAX, FLT_MAX);
	
	transform.m_rotation = Quat4f::CreateFromYawPitchRoll({ToRadians(rotation[0]), ToRadians(rotation[1]), ToRadians(rotation[2])}); // * (qZ));
	
	transform.m_position = { position[0], position[1], position[2], 1.f };
	transform.m_scale	 = { scale[0], scale[1], scale[2], 1.f};
}
#else
void Transform::DisplayInEditor(Entity entity){}
#endif
