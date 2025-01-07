#include "ImguiHandler.h"

#include "Core/Engine.h"
#include "Core/WindowHandler.h"
#include "Rendering/RenderManager.h"
#include "Rendering/2D/SpriteHandler.h"
#include "SceneManager.h"
#include "Rendering/Models/ModelData.h"
#include "Core/D3D12Framework.h"
#include "Core/FrameResource.h"
#include "Rendering/Models/ModelHandler.h"
#include "Components/Light/DirectionalLight.h"
#include "Rendering/TextureHandler.h"

#include "Components/2D/Text.h"
#include "Components/GameEntity.h"
#include "Components/2D/Sprite.h"
#include "Components/Transform.h"

#include "Core/input.hpp"

#include <tchar.h>
#include "imgui/imgui.h"

ImguiHandler::ImguiHandler(Engine& engine) :
	m_engine(engine),
	m_currentEntity(),
	m_entitySelected(false),
	m_name(nullptr),
	m_tag(nullptr),
	m_sceneLoader(engine)
{
	FileExplorer::FileType textures = { L".dds", L"Sprites\\" };
	m_fileExtensions.emplace("Texture", textures);

	FileExplorer::FileType models = { L".fbx", L"Models\\" };
	m_fileExtensions.emplace("Model", models); 	

	FileExplorer::FileType  scenes = { L".json", L"Scenes\\" };
	m_fileExtensions.emplace("Scenes", scenes);

	m_componentList = {
		"Model",
		"DirectionalLight"
	};
}
ImguiHandler::~ImguiHandler()
{
    m_name = nullptr;
    m_tag  = nullptr;
}

void ImguiHandler::Init()
{
	m_rightWindow.m_height = WindowHandler::WindowData().m_h;
	m_rightWindow.m_width = 400;
	m_rightWindow.m_positon.x = static_cast<float>(WindowHandler::WindowData().m_w - (m_rightWindow.m_width/2));
	m_rightWindow.m_positon.y = static_cast<float>((m_rightWindow.m_height / 2) + 19);

	m_leftWindow.m_height = WindowHandler::WindowData().m_h / 2;
	m_leftWindow.m_width = 400;
	m_leftWindow.m_positon.x = static_cast<float>(m_leftWindow.m_width / 2);
	m_leftWindow.m_positon.y = static_cast<float>((m_leftWindow.m_height / 2) + 19);

	m_tag  = new char;
	m_name = new char;

	SceneManager& scene = m_engine.GetSceneManager();
	entt::registry& reg = scene.Registry();

	auto dirLightList = reg.view<DirectionalLight, Transform>();

	entt::entity ent;
	for (auto entity : dirLightList)
		ent = entity;
	
	Transform& transform = reg.get<Transform>(ent);
	m_dirLightRot	   = transform.m_rotation.ToEuler();
	m_dirLightRot	   = { ToDegrees(m_dirLightRot.x), ToDegrees(m_dirLightRot.y), ToDegrees(m_dirLightRot.z) };
	m_dirLightLastRot  = m_dirLightRot;
}

void ImguiHandler::Update(const float dt)
{
	D3D12Framework&  framework   = m_engine.GetFramework();
	RenderManager& renderManager = m_engine.GetRenderManager();
	SceneManager& scene = m_engine.GetSceneManager();
	entt::registry& reg = scene.Registry();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Scene Lighting")) {	
			ImGui::Text("Directional Lighting");
			auto dirLightList = reg.view<DirectionalLight, Transform, GameEntity>();
			entt::entity ent;
			for (auto entity : dirLightList)
				ent = entity;

			DirectionalLight& light = reg.get<DirectionalLight>(ent);
			Transform&        transform = reg.get<Transform>(ent);

			float lightColor[3] = {light.m_lightColor.x, light.m_lightColor.y, light.m_lightColor.z};
			ImGui::ColorPicker3("Light Color", &lightColor[0]);

			float lightStrength = light.m_lightColor.w;
			ImGui::DragFloat("Light Strength", &lightStrength, 0.01f, 0.0f, 10.0f);

			float ambientColor[3] = {light.m_ambientColor.x, light.m_ambientColor.y, light.m_ambientColor.z};
			ImGui::ColorPicker3("Ambient Color", &ambientColor[0]);

			float ambientStrength = light.m_ambientColor.w;
			ImGui::DragFloat("Ambient Strength", &ambientStrength, 0.01f, 10.0f);

			light.m_lightColor   = { lightColor[0], lightColor[1], lightColor[2], lightStrength };
			light.m_ambientColor = { ambientColor[0], ambientColor[1], ambientColor[2], ambientStrength };

			float rotation[3] = { m_dirLightRot.x, m_dirLightRot.y, m_dirLightRot.z };
			if (ImGui::DragFloat3("Light Direction", &rotation[0], 0.1f, -0.001f, 360.001f)) {
				rotation[0] = rotation[0] > 360.0f ? 0.0f : rotation[0] < 0.0f ? 360.0f : rotation[0];
				rotation[1] = rotation[1] > 360.0f ? 0.0f : rotation[1] < 0.0f ? 360.0f : rotation[1];
				rotation[2] = rotation[2] > 360.0f ? 0.0f : rotation[2] < 0.0f ? 360.0f : rotation[2];

				m_dirLightRot   = { rotation[0], rotation[1], rotation[2] };
				Vect3f deltaRot = m_dirLightRot - m_dirLightLastRot; 
				
				Quat4f ddeltaQuat    = DirectX::XMQuaternionRotationRollPitchYaw(ToRadians(deltaRot.x), ToRadians(deltaRot.y), ToRadians(deltaRot.z));
				transform.m_rotation = DirectX::XMQuaternionMultiply(transform.m_rotation, ddeltaQuat);
				
				m_dirLightLastRot = m_dirLightRot;
			}
			
			float lightPosition[3] = { transform.m_position.x, transform.m_position.y, transform.m_position.z };
			ImGui::DragFloat3("Light Position", &lightPosition[0], 0.1f);
			transform.m_position = { lightPosition[0], lightPosition[1], lightPosition[2] };

			//ImGui::Text("Volumetric Lighting");
			//VolumetricLight& vl = renderManager.GetVolumetricLight();
			
			//int steps = vl.GetVolumetricData().m_numberOfSteps;
			//ImGui::DragInt("Number Of Steps", &steps, 5.0f, 10, 1000);
			//
			//float gScattering = vl.GetVolumetricData().m_gScattering;
			//ImGui::DragFloat("G Scattering", &gScattering, 0.01f, -1.0f, 1.0f);
			//
			//float scatteringStr = vl.GetVolumetricData().m_scatteringStrength;
			//ImGui::DragFloat("Scattering Strength", &scatteringStr, 0.01f, 1.5f, 10.0f);
			//
			//vl.SetVolumetricData(steps, gScattering, scatteringStr);

			ImGui::EndMenu();
		}


		//if (ImGui::BeginMenu("File")) {
		//	if (ImGui::MenuItem("Load Scene", "CTRL+O")) {
		//		std::wstring scene = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_GET, m_fileExtensions["Scenes"]);
		//		if (!scene.empty()) {
		//
		//			// This section is not my proudest programming moment...
		//			// Definetly Gonna rewrite this whole section when I have remade
		//			// Scenes and how they function.
		//
		//			bool loadScene = false;
		//
		//			std::string sceneInString = { scene.begin(), scene.end() };
		//			if (sceneInString != m_engine.GetSceneManager().GetCurrentScene().SceneName()) {
		//				loadScene = true;
		//			}
		//			else {
		//				if (ImGui::BeginPopupContextWindow()) {
		//					ImGui::Text("Want to save scene before loading the same/a new one?");
		//
		//					if (ImGui::Button("Save")) {
		//						SaveScene(reg);
		//						loadScene = true;
		//					}
		//					ImGui::SameLine();
		//					if(ImGui::Button("Load"))
		//						loadScene = true;
		//
		//					ImGui::SameLine();
		//					if (ImGui::Button("Close")){}
		//					
		//					ImGui::EndPopup();
		//				}
		//			}
		//
		//			if (loadScene) {
		//				// This is very complicated for no reason atm, something i really want to fix 
		//				// once i get a good idea how to handle scenes between edtior and game
		//				entt::entity camEntt = m_engine.GetSceneManager().GetCurrentScene().GetMainCamera();
		//				Camera& camera = reg.get<Camera>(camEntt);
		//				Transform& transform = reg.get<Transform>(camEntt);
		//				Object& obj = reg.get<Object>(camEntt);
		//
		//				Scene& newScene = m_engine.GetSceneManager().CreateEmptyScene(sceneInString);
		//				if (!newScene.Registry().empty())
		//					newScene.Registry().clear();
		//
		//				entt::registry& newReg = newScene.Registry();
		//				entt::entity newCam = newReg.create();
		//				newReg.emplace<Camera>(newCam, camera);
		//				newReg.emplace<Transform>(newCam, transform);
		//				newReg.emplace<Object>(newCam, obj);
		//
		//				m_engine.GetSceneManager().SetScene(sceneInString, newCam);
		//				m_sceneLoader.LoadScene(sceneInString, newReg);
		//
		//				m_itemsHasBeenSelected = false;
		//			}
		//		}
		//	}
		//
		//	if (ImGui::MenuItem("Save Scene", "CTRL+S")) {
		//		SaveScene(reg);
		//	}
		//	
		//	if (ImGui::MenuItem("Save Scene as", "F12")) {
		//		SaveSceneAs(reg);
		//	}
		//
		//	ImGui::EndMenu();
		//}
	}
	ImGui::EndMainMenuBar();	
	
	StaticWindows();
}


void ImguiHandler::StaticWindows()
{
	ImGuiWindowFlags staticWindowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	entt::registry& reg = m_engine.GetSceneManager().Registry();
	//* Left Side window Begin
	{
		ImGui::SetNextWindowSize({ (float)m_leftWindow.m_width, (float)m_leftWindow.m_height });
		ImGui::SetNextWindowPos({ m_leftWindow.m_positon.x, m_leftWindow.m_positon.y }, 0, { 0.5f, 0.5f });
		ImGui::SetNextWindowBgAlpha(1.f);
		bool isOpen = true;

		if (ImGui::Begin("Scene View", &isOpen, staticWindowFlags)) {
			if (ImGui::Button("Create Empty GameEntity")) {
				m_currentEntity = m_engine.GetSceneManager().CreateBasicEntity("Empty Entity", false).m_entity;
			}

			ImGui::Separator();

			if (ImGui::BeginListBox("##", { (float)m_leftWindow.m_width, (float)m_leftWindow.m_width })) {
				auto scene = reg.view<Transform, GameEntity>();
				entt::entity previousEntity;
				for (auto entity : scene) {
	
					GameEntity& obj = reg.get<GameEntity>(entity);
					bool isSelected = (entity == m_currentEntity);
					if (ImGui::Selectable(obj.m_name.c_str(), isSelected)) {
						m_currentEntity = entity;
						m_currentMesh = 0;
						Transform& transform = reg.get<Transform>(entity);

						if (m_removeEntity) {
							if (previousEntity != entity) {
								m_currentEntity = previousEntity;
							}

							reg.destroy(entity);
							m_removeEntity = false;
						}

						if (!m_itemsHasBeenSelected)
							m_itemsHasBeenSelected = true;
					}

					previousEntity = entity;
				}

				ImGui::EndListBox();
			}

			ImGui::End();
		}
	}
	//* Left Side window End

	//* Right side window begin
	{
		ImGui::SetNextWindowSize({ (float)m_rightWindow.m_width, (float)m_rightWindow.m_height });
		ImGui::SetNextWindowPos({ m_rightWindow.m_positon.x, m_rightWindow.m_positon.y }, 0, { 0.5f, 0.5f });
		ImGui::SetNextWindowBgAlpha(1.f);
		bool isOpen = true;

		if (ImGui::Begin("Component Window", &isOpen, staticWindowFlags)) {

			if (m_itemsHasBeenSelected) {
				if(ObjectSettings(reg))
					ImGui::Separator();
				if(TransformSettings(reg))
					ImGui::Separator();
				if (DirectionalLightSettings(reg))
					ImGui::Separator();
				if (ModelDataSettings(reg))
					ImGui::Separator();

				if (ImGui::Button("Add Component")) {
					ImGui::OpenPopup("ComponentList");
				}
				if (ImGui::BeginPopup("ComponentList")) {	
					std::string selectedComponent = "";
				
					for (UINT i = 0; i < m_componentList.size(); i++)
					{
						bool selected = (selectedComponent == m_componentList[i]);
						if (ImGui::Selectable(m_componentList[i].c_str(), selected)) {
							selectedComponent = m_componentList[i];
							
							if (selectedComponent == "Model") {
								if (!reg.try_get<Model>(m_currentEntity)) {
									reg.emplace<Model>(m_currentEntity, 0);
								}
								break;
							}
				
							if (selectedComponent == "DirectionalLight") {
								if (!reg.try_get<DirectionalLight>(m_currentEntity)) {
									reg.emplace<DirectionalLight>(m_currentEntity);
								}
								break;
							}
				
						}
					}
				
					ImGui::EndPopup();
				}
			}
		
			ImGui::End();
		}
	}
	//* Right side window end
}
void ImguiHandler::SaveScene(entt::registry& reg)
{
	//if (!m_sceneLoader.CurrentScene().empty()) {
	//	m_sceneLoader.SaveScene(m_sceneLoader.CurrentScene(), reg);
	//}
	//else {
	//	std::wstring scene = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_SAVE, m_fileExtensions["Scenes"]);
	//	m_sceneLoader.SaveScene({ scene.begin(), scene.end() }, reg);
	//}
}
void ImguiHandler::SaveSceneAs(entt::registry& reg)
{
	//std::wstring scene = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_SAVE, m_fileExtensions["Scenes"]);
	//m_sceneLoader.SaveScene({ scene.begin(), scene.end() }, reg);
}
bool ImguiHandler::ModelDataSettings(entt::registry& reg)
{
	Model* model = reg.try_get<Model>(m_currentEntity);
	if (model) {
		if (ImGui::CollapsingHeader("ModelData")) {
			if (ImGui::Button("Set Model")) {
				std::wstring newModel = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_GET, m_fileExtensions["Model"]);
				if (!newModel.empty()) {
					UINT newModelID = m_engine.GetModelHandler().LoadModel(newModel, "").m_modelID;
					if (newModelID != 0) {
						model->m_modelID = newModelID;
					}
				}
			}
			ImGui::Separator();

			if(model->m_modelID != 0) {
				ModelData& data = m_engine.GetModelHandler().GetLoadedModelInformation(model->m_modelID);
				
				ImGui::Text("Model Name: ");
				ImGui::SameLine();
				ImGui::Text(data.Name().c_str());

				if (m_currentMesh > data.GetMeshes().size() - 1)
					m_currentMesh = 0;
				
				int lastMesh = m_currentMesh;
				ImGui::SliderInt("Mesh", &m_currentMesh, 0, (UINT)data.GetMeshes().size() - 1);
				ModelData::Mesh& currentMesh = data.GetMeshes()[m_currentMesh];

				bool renderOnlyMesh = m_renderOnlyMesh;
				ImGui::Checkbox("Render Only Mesh", &renderOnlyMesh);
				if (renderOnlyMesh != m_renderOnlyMesh || m_currentMesh != lastMesh) {
					for (int i = 0; i < data.GetMeshes().size(); i++)
					{
						if (i == m_currentMesh) {
							data.GetMeshes()[i].m_renderMesh = true;
							continue;
						}

						data.GetMeshes()[i].m_renderMesh = !renderOnlyMesh;
					}

					lastMesh		 = m_currentMesh;
					m_renderOnlyMesh = renderOnlyMesh;
				}

				Material& material = currentMesh.m_material;
				float roughness = material.m_roughness;
				float metallic = material.m_shininess;
				float emissive = material.m_emissvie;
				float color[3] = { material.m_color[0], material.m_color[1], material.m_color[2] };

				//ImGui::ColorEdit3("Albedo Color", color);
				//{
				//	std::wstring albedo;
				//	if (ImGui::Button("Albedo")) {
				//		albedo = SelectTexture(material.m_albedo);
				//	}
				//	else
				//		albedo = m_engine.GetTextureHandler().GetTextureData(material.m_albedo).m_texturePath;
				//
				//	ImGui::SameLine();
				//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(albedo);
				//	ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
				//}
				//
				//{
				//	std::wstring metallicMap;
				//	if (ImGui::Button("Metallic Map")) {
				//		metallicMap = SelectTexture(material.m_metallicMap);
				//	}
				//	else
				//		metallicMap = m_engine.GetTextureHandler().GetTextureData(material.m_metallicMap).m_texturePath;
				//
				//	ImGui::SameLine();
				//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(metallicMap);
				//	ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
				//}
				//ImGui::SameLine();
				//ImGui::DragFloat("Metallic",  &metallic, 0.01f, 0.0f,  2.f);
				//
				//{
				//	std::wstring roughnessMap;
				//	if (ImGui::Button("Roughness Map")) {
				//		roughnessMap = SelectTexture(material.m_roughnessMap);
				//	}
				//	else
				//		roughnessMap = m_engine.GetTextureHandler().GetTextureData(material.m_roughnessMap).m_texturePath;
				//
				//	ImGui::SameLine();
				//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(roughnessMap);
				//	ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
				//}
				//ImGui::SameLine();
				ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.1f,  2.f);
				ImGui::DragFloat("Metallic",  &metallic,  0.01f, 0.0f,  2.f);
				ImGui::DragFloat("Emissive",  &emissive,  0.01f, 0.0f,  1.f);
				
				//{
				//	std::wstring normmal;
				//	if (ImGui::Button("Normal Map")) {
				//		normmal = SelectTexture(material.m_normal);
				//	}
				//	else
				//		normmal = m_engine.GetTextureHandler().GetTextureData(material.m_normal).m_texturePath;
				//
				//	ImGui::SameLine();
				//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(normmal);
				//	ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
				//}
				//{
				//	std::wstring height;
				//	if (ImGui::Button("Height Map")) {
				//		height = SelectTexture(material.m_heightMap);
				//	}
				//	else
				//		height = m_engine.GetTextureHandler().GetTextureData(material.m_heightMap).m_texturePath;
				//
				//	ImGui::SameLine();
				//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(height);
				//	ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
				//}
				//{
				//	std::wstring ao;
				//	if (ImGui::Button("AO Map")) {
				//		ao = SelectTexture(material.m_aoMap);
				//	}
				//	else
				//		ao = m_engine.GetTextureHandler().GetTextureData(material.m_aoMap).m_texturePath;
				//
				//	ImGui::SameLine();
				//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(ao);
				//	ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
				//}

				material.m_roughness = roughness;
				material.m_shininess = metallic;
				material.m_emissvie  = emissive;
				memcpy(material.m_color, color, sizeof(float) * 3);
			}
		}
			return true;
	}

	return false;
}

bool ImguiHandler::ObjectSettings(entt::registry& reg)
{	
	GameEntity& obj = reg.get<GameEntity>(m_currentEntity);
	ImGui::Text(obj.m_name.c_str());
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Game Entity")) {

		int len = (int)strlen(obj.m_tag.c_str());
		memcpy(m_tag, obj.m_tag.c_str(), len + 1);
		ImGui::InputText("Tag", m_tag, 30);
		obj.m_tag = m_tag;

		bool isStatic = obj.m_static;
		ImGui::Checkbox("Static", &isStatic);
		obj.m_static = isStatic;

		//const char* stateCombo[] = { "ACTIVE", "NOT_ACTIVE" };
		const char* currentState = nullptr;
		if (ImGui::BeginCombo("State", currentState)) {
			for (UINT i = 0; i < m_stateNames.size(); i++)
			{
				bool isSelected = (currentState == m_stateNames[i].c_str());

				if (ImGui::Selectable(m_stateNames[i].c_str(), isSelected))
					currentState = m_stateNames[i].c_str();

				if (isSelected) {
					ImGui::SetItemDefaultFocus();

					if (currentState == m_stateNames[0]) {
						obj.m_state = GameEntity::STATE::ACTIVE;
					}

					if (currentState == m_stateNames[1]) {
						obj.m_state = GameEntity::STATE::NOT_ACTIVE;
					}

					if (currentState == m_stateNames[2]) {
						obj.m_state = GameEntity::STATE::DESTROY;
					}
				}
			}
			ImGui::EndCombo();
		}

		return true;

	}

	return false;
}
bool ImguiHandler::TransformSettings(entt::registry& reg)
{
	Transform& transform = reg.get<Transform>(m_currentEntity);
	if (ImGui::CollapsingHeader("Transform")) {
		
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

		return true;
	}

	return false;
}

bool ImguiHandler::DirectionalLightSettings(entt::registry& reg)
{
	DirectionalLight* light = reg.try_get<DirectionalLight>(m_currentEntity);
	if (light) {
		if (ImGui::CollapsingHeader("Directional Light")) {
			float lightColor[3] = { light->m_lightColor.x, light->m_lightColor.y, light->m_lightColor.z };
			ImGui::ColorEdit3("Light Color", lightColor);

			float lightStr = light->m_lightColor.w;
			ImGui::DragFloat("Light Strength", &lightStr, 0.1f, 0.f, 20.f);
			light->m_lightColor.w = lightStr;

			light->m_lightColor = { lightColor[0], lightColor[1], lightColor[2], lightStr };

			float ambientColor[4] = { light->m_ambientColor.x, light->m_ambientColor.y, light->m_ambientColor.z, light->m_ambientColor.w };
			ImGui::ColorEdit3("Ambient Color", ambientColor);

			float ambientStr = light->m_ambientColor.w;
			ImGui::DragFloat("Ambient Strength", &ambientStr, 0.1f, 0.f, 20.f);
			light->m_ambientColor.w = ambientStr;

			light->m_ambientColor = { ambientColor[0], ambientColor[1], ambientColor[2], ambientStr };

			return true;
		}
	}

	return false;
}

std::wstring ImguiHandler::SelectTexture(UINT& texture)
{
	std::wstring newAlbedo = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_GET, m_fileExtensions["Texture"]);
	if (!newAlbedo.empty()) {
		texture = m_engine.GetTextureHandler().GetTexture(newAlbedo);
		if (texture == 0) {
			texture = m_engine.GetTextureHandler().CreateTexture(newAlbedo);
		}

		return newAlbedo;
	}
	else
		return m_engine.GetTextureHandler().GetTextureData(texture).m_texturePath;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ImguiHandler::AddImguiImage(std::wstring path)
{
	
	//for (UINT i = 0; i < m_imguiTextures.size(); i++)
	//{
	//	if (path == m_imguiTextures[i].m_imagePath)
	//		return m_imguiTextures[i].m_srvGpuHandle;
	//}
	//
	////if(!m_engine.GetFramework().CmdListIsRecording())
	////	m_engine.GetFramework().ResetCommandListAndAllocator(nullptr, L"ImguiHandler: Line 570");
	//
	//ImguiTexture texture;
	//
	//ID3D12Device* device = m_engine.GetFramework().GetDevice();
	//ID3D12GraphicsCommandList* cmdList = m_engine.GetFramework().CurrentFrameResource()->CmdList();
	//
	//CD3DX12_RESOURCE_BARRIER barrier = LoadATextures(
	//	path,
	//	device,
	//	cmdList,
	//	&texture.m_texture
	//);
	//
	//cmdList->ResourceBarrier(1, &barrier);
	//m_engine.GetFramework().ExecuteCommandList();
	//m_engine.GetFramework().WaitForGPU();
	//
	//ID3D12DescriptorHeap* imguiDesc = m_engine.GetFramework().GetImguiHeap();
	//
	//CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	//CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;
	//
	//UINT inrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//srvGpuHandle.InitOffsetted(imguiDesc->GetGPUDescriptorHandleForHeapStart(), static_cast<INT>(m_imguiTextures.size()) + 1, inrementSize);
	//srvCpuHandle.InitOffsetted(imguiDesc->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(m_imguiTextures.size()) + 1, inrementSize);
	//
	//DirectX::CreateShaderResourceView(
	//	device,
	//	texture.m_texture,
	//	srvCpuHandle
	//);
	//
	//texture.m_imagePath = path;
	//texture.m_srvGpuHandle = srvGpuHandle;
	//m_imguiTextures.emplace_back(texture);
	//
	//
	//return m_imguiTextures[m_imguiTextures.size() - 1].m_srvGpuHandle;

	return CD3DX12_GPU_DESCRIPTOR_HANDLE();
}