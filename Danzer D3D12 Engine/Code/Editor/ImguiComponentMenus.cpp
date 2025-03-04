#include "EditorPCH.h"

#include "ImguiComponentMenus.h"

#include "Components/AllComponents.h"
#include "Rendering/Camera.h"

#include "imgui/imgui.h"

void ImguiComponentMenus::DisplayComponentData(GameEntity& gameEntity)
{
	char* name = new char;
	int length = (int)strlen(gameEntity.m_name.c_str());
	memcpy(name, gameEntity.m_name.c_str(), length + 1);

	ImGui::InputText("Entity Name", name, 32);
	gameEntity.m_name = name;

	static bool isStatic = gameEntity.m_static;
	ImGui::Selectable("Static", isStatic);
	gameEntity.m_static = isStatic;

	static std::string states[3] = { "Active", "Deactive", "Destroy" };
	if (ImGui::BeginCombo("Entity State", states[gameEntity.m_state].c_str())){
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

void ImguiComponentMenus::DisplayComponentData(Transform& transform)
{
	Vect3f euler      = transform.m_rotation.ToEuler();	

	float position[3] = { transform.m_position.x, transform.m_position.y, transform.m_position.z };
	float scale[3]    = { transform.m_scale.x,    transform.m_scale.y,    transform.m_scale.z };
	float rotation[3] = { transform.m_lastRotCheck.x, transform.m_lastRotCheck.y, transform.m_lastRotCheck.z };

	ImGui::DragFloat3("Position", position, 0.01f, -FLT_MAX, FLT_MAX);
	if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f, -0.00001f, 360.001f)) {
		rotation[0] = rotation[0] > 360.0f ? 0.0f : rotation[0] < 0.0f ? 360.0f : rotation[0];
		rotation[1] = rotation[1] > 360.0f ? 0.0f : rotation[1] < 0.0f ? 360.0f : rotation[1];
		rotation[2] = rotation[2] > 360.0f ? 0.0f : rotation[2] < 0.0f ? 360.0f : rotation[2];
		//m_dirLightRot = { rotation[0], rotation[1], rotation[2] };
		//Vect3f deltaRot = m_dirLightRot - m_dirLightLastRot;
		//
		//Quat4f ddeltaQuat = DirectX::XMQuaternionRotationRollPitchYaw(ToRadians(deltaRot.x), ToRadians(deltaRot.y), ToRadians(deltaRot.z));
		//transform.m_rotation = DirectX::XMQuaternionMultiply(transform.m_rotation, ddeltaQuat);
		//
		//m_dirLightLastRot = m_dirLightRot;
	}
	ImGui::DragFloat3("Scale", scale, 0.01f, -FLT_MAX, FLT_MAX);

	transform.m_rotation = Quat4f::CreateFromYawPitchRoll({ ToRadians(rotation[0]), ToRadians(rotation[1]), ToRadians(rotation[2]) }); // * (qZ));

	transform.m_position = { position[0], position[1], position[2], 1.f };
	transform.m_scale    = { scale[0], scale[1], scale[2], 1.f };
}

void ImguiComponentMenus::DisplayComponentSelection(entt::entity entity)
{
	//if (!reg.try_get<Model>(entity))
	//	if (ImGui::Selectable(typeid(Model).name(), nullptr)) 
	//		reg.emplace<Model>(entity);

	bool selected = false;

	if (!Reg::Instance()->TryGet<PointLight>(entity))
		if (ImGui::Selectable(typeid(PointLight).name(), selected))
			Reg::Instance()->Emplace<PointLight>(entity);

	if (!Reg::Instance()->TryGet<Camera>(entity))
		if (ImGui::Selectable(typeid(Camera).name(), selected))
			Reg::Instance()->Emplace<Camera>(entity);
}
