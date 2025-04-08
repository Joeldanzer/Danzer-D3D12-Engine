#include "stdafx.h"
#include "Transform.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void Transform::DisplayInEditor(Entity entity)
{
	Transform& transform = REGISTRY->Get<Transform>(entity);

	float position[3] = {transform.m_position.x, transform.m_position.y, transform.m_position.z };
	float scale[3]	  = {transform.m_scale.x,    transform.m_scale.y,    transform.m_scale.z	};

	ImGui::DragFloat3("Position", position, 0.01f, -FLT_MAX, FLT_MAX);
	ImGui::DragFloat3("Rotation", &transform.m_editorRotation.x, 0.1f, -180.1, 180.1f);
	float* rot = &transform.m_editorRotation.x;
	for (uint16_t i = 0; i < 3; i++) {
		if (rot[i] > 180.0f)
			rot[i] = -179.999f;

		if (rot[i] < -180.0f)
			rot[i] = 179.999f;
	}
	
	//transform.m_editorRotation.y = transform.m_editorRotation.y > 180.01f ? -180.0f : transform.m_editorRotation.y < -180.01f ? 180.0f : transform.m_editorRotation.y;
	//transform.m_editorRotation.z = transform.m_editorRotation.z > 180.01f ? -180.0f : transform.m_editorRotation.z < -180.01f ? 180.0f : transform.m_editorRotation.z;
	//
	//if (transform.m_editorRotation.x < -180.0f)
	//	int s = 1;

	transform.m_rotation = EditorQuatRotate(transform.m_rotation, transform.m_editorRotation, transform.m_lastEditorRotation);

	transform.m_lastEditorRotation = transform.m_editorRotation;

	ImGui::DragFloat3("Scale", scale, 0.01f, -FLT_MAX, FLT_MAX);
	
	transform.m_position = { position[0], position[1], position[2], 1.f };
	transform.m_scale	 = { scale[0], scale[1], scale[2], 1.f};
}
#else
void Transform::DisplayInEditor(Entity entity){}
#endif
