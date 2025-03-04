#include "stdafx.h"
#include "DirectionalLight.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
#include "Components/Transform.h"
void DirectionalLight::DisplayInEditor(Entity entity)
{
	DirectionalLight& light = REGISTRY->Get<DirectionalLight>(entity);
	Transform& transform    = REGISTRY->Get<Transform>(entity);

	static Vect3f lightRot     = transform.m_rotation;
	static Vect3f lastLightRot = lightRot;

	float lightColor[3] = { light.m_lightColor.x, light.m_lightColor.y, light.m_lightColor.z };
	ImGui::ColorPicker3("Light Color", &lightColor[0]);

	float lightStrength = light.m_lightColor.w;
	ImGui::DragFloat("Light Strength", &lightStrength, 0.01f, 0.0f, 10.0f);

	float ambientColor[3] = { light.m_ambientColor.x, light.m_ambientColor.y, light.m_ambientColor.z };
	ImGui::ColorPicker3("Ambient Color", &ambientColor[0]);
	  
	float ambientStrength = light.m_ambientColor.w;
	ImGui::DragFloat("Ambient Strength", &ambientStrength, 0.01f, 10.0f);

	light.m_lightColor = { lightColor[0],   lightColor[1],   lightColor[2],   lightStrength };
	light.m_ambientColor = { ambientColor[0], ambientColor[1], ambientColor[2], ambientStrength };

	float rotation[3] = { lightRot.x, lightRot.y, lightRot.z };
	if (ImGui::DragFloat3("Light Direction", &rotation[0], 0.1f, -0.001f, 360.001f)) {
		rotation[0] = rotation[0] > 360.0f ? 0.0f : rotation[0] < 0.0f ? 360.0f : rotation[0];
		rotation[1] = rotation[1] > 360.0f ? 0.0f : rotation[1] < 0.0f ? 360.0f : rotation[1];
		rotation[2] = rotation[2] > 360.0f ? 0.0f : rotation[2] < 0.0f ? 360.0f : rotation[2];

		lightRot = { rotation[0], rotation[1], rotation[2] };
		Vect3f deltaRot = lightRot - lastLightRot;

		Quat4f ddeltaQuat    = DirectX::XMQuaternionRotationRollPitchYaw(ToRadians(deltaRot.x), ToRadians(deltaRot.y), ToRadians(deltaRot.z));
		transform.m_rotation = DirectX::XMQuaternionMultiply(transform.m_rotation, ddeltaQuat);

		lastLightRot = lightRot;
	}
}
#else
void DirectionalLight::DisplayInEditor(Entity entity){}
#endif
