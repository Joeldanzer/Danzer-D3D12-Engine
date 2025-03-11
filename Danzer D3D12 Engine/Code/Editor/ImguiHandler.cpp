#include "ImguiHandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

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
#include "Rendering/Camera.h"

#include "Components/2D/Text.h"

#include "Components/GameEntity.h"
#include "Components/2D/Sprite.h"
#include "Components/Transform.h"

#include "Rendering/Screen Rendering/Textures/TextureRenderingHandler.h"
#include "Rendering/Screen Rendering/Textures/FullscreenTexture.h"

#include "Core/input.hpp"

#include <tchar.h>

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
	m_tag  = new char;
	m_name = new char;

	SceneManager& scene = m_engine.GetSceneManager();

	auto dirLightList = Reg::Instance()->GetRegistry().view<DirectionalLight, Transform>();

	entt::entity ent;
	for (entt::entity entity : dirLightList)
		ent = entity;
	
	Transform& transform = Reg::Instance()->Get<Transform>(ent);
	m_dirLightRot	   = transform.m_rotation.ToEuler();
	m_dirLightRot	   = { ToDegrees(m_dirLightRot.x), ToDegrees(m_dirLightRot.y), ToDegrees(m_dirLightRot.z) };
	m_dirLightLastRot  = m_dirLightRot;
}

void ImguiHandler::Update(const float dt)
{
	D3D12Framework& framework     = m_engine.GetFramework();
	RenderManager&  renderManager = m_engine.GetRenderManager();
	SceneManager&   scene		  = m_engine.GetSceneManager();

#if defined(_DEBUG) || defined(EDITOR_DEBUG_VIEW) // General Menu bar for DEBUG Game & Editor view.
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Scene Lighting")) {	
			ImGui::Text("Directional Lighting");
			auto dirLightList = Reg::Instance()->GetRegistry().view<DirectionalLight, Transform, GameEntity>();
			entt::entity ent;
			for (auto entity : dirLightList)
				ent = entity;

			DirectionalLight& light     = REGISTRY->Get<DirectionalLight>(ent);
			light.DisplayInEditor(ent);
			
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
	}
	ImGui::EndMainMenuBar();	
#endif

#if EDITOR_DEBUG_VIEW // Still want to keep some functions available
	SetUpDockingWindows();
	DrawSceneToWindow(Reg::Instance()->Get<Camera>(scene.GetMainCamera()));

	StaticWindows();
#endif
}

void ImguiHandler::SetUpDockingWindows()
{
	static ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize({ viewport->Size.x, viewport->Size.y / (3.0f/4.0f)});
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
		windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
		ImGui::Begin("TopDockSpace", nullptr, windowFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockSpaceID, {0.0f, 0.0f}, dockSpaceFlags);


			static bool setTopDocks = false;
			if (!setTopDocks) {
				setTopDocks = true;
				ImGui::DockBuilderRemoveNode(dockSpaceID);
				ImGui::DockBuilderAddNode(dockSpaceID, dockSpaceFlags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockSpaceID, viewport->Size);
				
				auto up			 = ImGui::DockBuilderSplitNode(dockSpaceID,  ImGuiDir_Up,    0.1f,  nullptr, &dockSpaceID);
				auto right		 = ImGui::DockBuilderSplitNode(dockSpaceID,  ImGuiDir_Right, 0.80f, nullptr, &up);				
				auto secondRight = ImGui::DockBuilderSplitNode(right,        ImGuiDir_Right, 0.2f,  nullptr, &right);

				ImGui::DockBuilderDockWindow("Scene View",		 up);
				ImGui::DockBuilderDockWindow("Viewport",		 right);
				ImGui::DockBuilderDockWindow("Component Window", secondRight);

				ImGui::DockBuilderFinish(dockSpaceID);
			}
		}		
		ImGui::End();	

		//ImGui::Begin("BottomDockSpace", nullptr, windowFlags);
		//ImGui::SetNextWindowPos(viewport->Pos);
		//ImGui::SetNextWindowSize({ viewport->Size.x, viewport->Size.y / (3.0f / 4.0f) });
		//ImGui::SetNextWindowViewport(viewport->ID);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
		//ImGui::End(); 
	}
}

void ImguiHandler::DrawSceneToWindow(Camera& viewPortCam)
{
	uint32_t frameIndex = m_engine.GetFramework().GetFrameIndex();
	const FullscreenTexture* sceneView = m_engine.GetRenderManager().GetTextureRendering().GetLastRenderedTexture();

	// We skip the first frame since this texture hasn't been rendered yet.
	if (sceneView != nullptr) {
		ImVec2 viewPortHalf = { WindowHandler::WindowData().m_w / 2.0f, WindowHandler::WindowData().m_h / 2.0f };
		ImGui::SetNextWindowSize(viewPortHalf);
		ImGui::Begin("Viewport", nullptr);

		Vect2f windowSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

		if (m_lastSceneToWindowSize != windowSize) {
			m_lastSceneToWindowSize = windowSize;

			viewPortCam.SetAspectRatio(windowSize.x / windowSize.y); // Only reconstruct cam when window has changed size
		}
			
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = m_engine.GetFramework().CbvSrvHeap().GET_GPU_DESCRIPTOR(sceneView->SRVOffsetID() + frameIndex);
		ImGui::Image(ImTextureID(srvHandle.ptr), {windowSize.x, windowSize.y});
	
		ImGui::End();
	}
}


void ImguiHandler::StaticWindows()
{
	//ImGuiWindowFlags staticWindowFlags =
	//	ImGuiWindowFlags_M |
	//	ImGuiWindowFlags_NoResize |
	//	ImGuiWindowFlags_NoCollapse;

	//ImVec2 windowSize = { (float)WindowHandler::WindowData().m_w, (float)WindowHandler::WindowData().m_h };

	//* Left Side window Begin
	{
		bool isOpen = true;

		if (ImGui::Begin("Scene View", nullptr)) {
			if (ImGui::Button("Create Empty GameEntity")) {
				if (REGISTRY->EntityWithNameExists(m_baseEntityName)) {
					auto it = std::find_if(m_baseEntityName.begin(), m_baseEntityName.end(), ::isdigit);
					if (it != m_baseEntityName.end()) {
						int value = std::atoi(std::string(1, *it).c_str());
						value++;
						
						m_baseEntityName.replace(it, it + 1, std::to_string(value));
					}
					else {
						m_baseEntityName += " " + std::to_string(1);
					}
				}
				m_currentEntity = REGISTRY->Create3DEntity(m_baseEntityName, false);
			}

			ImGui::Separator();

			if (ImGui::BeginListBox("##", ImGui::GetWindowSize())) {
				auto scene = Reg::Instance()->GetRegistry().view<Transform, GameEntity>();
				entt::entity previousEntity;
				for (auto entity : scene) {
					GameEntity& obj = REGISTRY->Get<GameEntity>(entity);
					bool isSelected = (entity == m_currentEntity);

					if (ImGui::Selectable(obj.m_name.empty() ? "##" : obj.m_name.c_str(), isSelected)) {
						m_currentEntity = entity;
						m_currentMesh = 0;
						Transform& transform = REGISTRY->Get<Transform>(entity);

						if (m_removeEntity) {
							if (previousEntity != entity) {
								m_currentEntity = previousEntity;
							}

							REGISTRY->DestroyEntity(entity);
							m_removeEntity = false;
						}

						if (!m_itemsHasBeenSelected)
							m_itemsHasBeenSelected = true;
					}

					previousEntity = entity;
					
				}
				ImGui::EndListBox();

			}

		}
	}
	ImGui::End();
	//* Left Side window End

	//* Right side window begin
	{
		ImGui::SetNextWindowBgAlpha(1.f);
		bool isOpen = true;

		if (ImGui::Begin("Component Window", &isOpen)) {
			if (m_itemsHasBeenSelected) {
				GameEntity& gameEntity = REGISTRY->Get<GameEntity>(m_currentEntity);
				
				for (uint32_t i = 0; i < gameEntity.m_emplacedComponents.size(); i++) {
					if(ImGui::CollapsingHeader(gameEntity.m_emplacedComponents[i].c_str(), ImGuiTreeNodeFlags_DefaultOpen))
						COMPONENT_ENTRY_REGISTER.DisplayComponent(m_currentEntity, gameEntity.m_emplacedComponents[i]);
				}

				ImGui::Separator();

				if (ImGui::Button("Add Component")) {
					ImGui::OpenPopup("ComponentList");
				}

				if (ImGui::BeginPopup("ComponentList")) {
					const std::map<std::string, BaseComponent*>& cReg = COMPONENT_ENTRY_REGISTER.GetComponentRegister();
					
					bool selectedComponent = false;
					for (auto const& it : cReg)
					{
						if (REGISTRY->HasComponent(m_currentEntity, it.first))
							continue;

						if (ImGui::Selectable(it.first.c_str(), selectedComponent)) {
							COMPONENT_ENTRY_REGISTER.EmplaceComponent(m_currentEntity, it.first);
						}
					}
				
					ImGui::EndPopup();
				}
			}
		
		}
		ImGui::End();
	}
	//* Right side window end
}
void ImguiHandler::SaveScene()
{
	//if (!m_sceneLoader.CurrentScene().empty()) {
	//	m_sceneLoader.SaveScene(m_sceneLoader.CurrentScene(), reg);
	//}
	//else {
	//	std::wstring scene = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_SAVE, m_fileExtensions["Scenes"]);
	//	m_sceneLoader.SaveScene({ scene.begin(), scene.end() }, reg);
	//}
}
void ImguiHandler::SaveSceneAs()
{
	//std::wstring scene = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_SAVE, m_fileExtensions["Scenes"]);
	//m_sceneLoader.SaveScene({ scene.begin(), scene.end() }, reg);
}

	//Model* model = reg.try_get<Model>(m_currentEntity);
	//if (model) {
	//	if (ImGui::CollapsingHeader("ModelData")) {
	//		if (ImGui::Button("Set Model")) {
	//			std::wstring newModel = m_fileExplorer.OpenFileExplorer(FILE_EXPLORER_GET, m_fileExtensions["Model"]);
	//			if (!newModel.empty()) {
	//				UINT newModelID = m_engine.GetModelHandler().LoadModel(newModel, "").m_modelID;
	//				if (newModelID != 0) {
	//					model->m_modelID = newModelID;
	//				}
	//			}
	//		}
	//		ImGui::Separator();
	//
	//		if(model->m_modelID != 0) {
	//			ModelData& data = m_engine.GetModelHandler().GetLoadedModelInformation(model->m_modelID);
	//			
	//			ImGui::Text("Model Name: ");
	//			ImGui::SameLine();
	//			ImGui::Text(data.Name().c_str());
	//
	//			if (m_currentMesh > data.GetMeshes().size() - 1)
	//				m_currentMesh = 0;
	//			
	//			int lastMesh = m_currentMesh;
	//			ImGui::SliderInt("Mesh", &m_currentMesh, 0, (UINT)data.GetMeshes().size() - 1);
	//			ModelData::Mesh& currentMesh = data.GetMeshes()[m_currentMesh];
	//
	//			bool renderOnlyMesh = m_renderOnlyMesh;
	//			ImGui::Checkbox("Render Only Mesh", &renderOnlyMesh);
	//			if (renderOnlyMesh != m_renderOnlyMesh || m_currentMesh != lastMesh) {
	//				for (int i = 0; i < data.GetMeshes().size(); i++)
	//				{
	//					if (i == m_currentMesh) {
	//						data.GetMeshes()[i].m_renderMesh = true;
	//						continue;
	//					}
	//
	//					data.GetMeshes()[i].m_renderMesh = !renderOnlyMesh;
	//				}
	//
	//				lastMesh		 = m_currentMesh;
	//				m_renderOnlyMesh = renderOnlyMesh;
	//			}
	//
	//			Material& material = currentMesh.m_material;
	//			float roughness = material.m_roughness;
	//			float metallic = material.m_shininess;
	//			float emissive = material.m_emissvie;
	//			float color[3] = { material.m_color[0], material.m_color[1], material.m_color[2] };

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
				//ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.1f,  2.f);
				//ImGui::DragFloat("Metallic",  &metallic,  0.01f, 0.0f,  2.f);
				//ImGui::DragFloat("Emissive",  &emissive,  0.01f, 0.0f,  1.f);
				
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

				//material.m_roughness = roughness;
				//material.m_shininess = metallic;
				//material.m_emissvie  = emissive;
				//memcpy(material.m_color, color, sizeof(float) * 3);
	//		}
	//	}
	//		return true;
	//}
	//


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
