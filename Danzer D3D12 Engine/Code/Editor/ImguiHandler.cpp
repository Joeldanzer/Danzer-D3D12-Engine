#include "ImguiHandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuizmo/ImGuizmo.h"

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
	//FileExplorer::FileType textures = { L".dds", L"Sprites\\" };
	//m_fileExtensions.emplace("Texture", textures);
	//
	//FileExplorer::FileType models = { L".fbx", L"Models\\" };
	//m_fileExtensions.emplace("Model", models); 	
	//
	//FileExplorer::FileType  scenes = { L".json", L"Scenes\\" };
	//m_fileExtensions.emplace("Scenes", scenes);

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
	ImGui::ShowDemoWindow();

#if defined(_DEBUG) || defined(EDITOR_DEBUG_VIEW) // General Menu bar for DEBUG Game & Editor view.
	D3D12Framework& framework     = m_engine.GetFramework();
	RenderManager&  renderManager = m_engine.GetRenderManager();
	SceneManager&   scene		  = m_engine.GetSceneManager();
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Scene Lighting")) {	
			ImGui::Text("Directional Lighting");
			auto dirLightList = Reg::Instance()->GetRegistry().view<DirectionalLight, Transform, GameEntity>();
			entt::entity ent;
			for (auto entity : dirLightList)
				ent = entity;

			DirectionalLight& light = REGISTRY->Get<DirectionalLight>(ent);
			light.DisplayInEditor(ent);
			
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();	
#endif

#if EDITOR_DEBUG_VIEW // Still want to keep some functions available
	SetUpDockingWindows();
	DisplayViewport();

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
		Vect2f windowSize   = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
		m_sceneViewSize		= windowSize;
		m_sceneViewPosition = { ImGui::GetWindowPos().x,  ImGui::GetWindowPos().y };

		if (m_lastSceneToWindowSize != windowSize) {
			m_lastSceneToWindowSize = windowSize;

			viewPortCam.SetAspectRatio(windowSize.x / windowSize.y); // Only reconstruct cam when window has changed size
		}
			
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = m_engine.GetFramework().CbvSrvHeap().GET_GPU_DESCRIPTOR(sceneView->SRVOffsetID() + frameIndex);
		ImGui::Image(ImTextureID(srvHandle.ptr), {windowSize.x, windowSize.y});
	
	}
}

void ImguiHandler::DisplayViewport()
{
	ImVec2 viewPortHalf = { WindowHandler::WindowData().m_w / 2.0f, WindowHandler::WindowData().m_h / 2.0f };
	ImGui::SetNextWindowSize(viewPortHalf);
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar);

	DrawSceneToWindow(REGISTRY->Get<Camera>(Engine::GetInstance().GetSceneManager().GetMainCamera()));
	ManipulateGameEntity();
	
	ImGui::End();
}

void ImguiHandler::ManipulateGameEntity()
{
	static ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;
	static ImGuizmo::MODE	   guizmoMode	   = ImGuizmo::WORLD;

	if (ImGui::BeginMenuBar()) {		
		if (ImGui::RadioButton("Tr", guizmoOperation == ImGuizmo::TRANSLATE))
			guizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rt", guizmoOperation == ImGuizmo::ROTATE))
			guizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Sc", guizmoOperation == ImGuizmo::SCALE))
			guizmoOperation = ImGuizmo::SCALE;
	
		ImGui::EndMenuBar();
	}

	if (m_itemsHasBeenSelected) {
		Transform& transform = REGISTRY->Get<Transform>(m_currentEntity);

		static Vect3f lastRotation = Vect3f::Zero;

		ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
		
		float newMatrix[4][4];
		ImGuizmo::RecomposeMatrixFromComponents(&transform.m_position.x, &transform.m_editorRotation.x, &transform.m_scale.x, &newMatrix[0][0]);
		Camera&    cam			= REGISTRY->Get<Camera>(Engine::GetInstance().GetSceneManager().GetMainCamera());
		Transform& camTransform = REGISTRY->Get<Transform>(Engine::GetInstance().GetSceneManager().GetMainCamera());

		ImGuizmo::SetRect(m_sceneViewPosition.x, m_sceneViewPosition.y, m_sceneViewSize.x, m_sceneViewSize.y);
		ImGuizmo::Manipulate(
			&camTransform.GetWorld().Invert().m[0][0],
			&cam.GetProjection().m[0][0],
			guizmoOperation, ImGuizmo::MODE::WORLD,
			&newMatrix[0][0]
		);
		
		ImGuizmo::DecomposeMatrixToComponents(&newMatrix[0][0], &transform.m_position.x, &transform.m_editorRotation.x, &transform.m_scale.x);
  		transform.m_rotation = EditorQuatRotate(transform.m_rotation, transform.m_editorRotation, transform.m_lastEditorRotation);
		
		transform.m_lastEditorRotation = transform.m_editorRotation;
	}
}


void ImguiHandler::StaticWindows()
{
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

			if (ImGui::Button("Destry Selected Entity")) {
				REGISTRY->DestroyEntity(m_currentEntity);
				m_itemsHasBeenSelected = false;
			}

			ImGui::Separator();

			if (ImGui::BeginListBox("##", ImGui::GetWindowSize())) {
				auto scene = Reg::Instance()->GetRegistry().view<Transform, GameEntity>();

				for (auto entity : scene) {
					GameEntity& obj = REGISTRY->Get<GameEntity>(entity);
					bool isSelected = (entity == m_currentEntity);

					if (ImGui::Selectable(obj.m_name.empty() ? "##" : obj.m_name.c_str(), isSelected)) {
						m_currentEntity  = entity;
			
						if (!m_itemsHasBeenSelected) {
							m_itemsHasBeenSelected = true;
							Transform transform = REGISTRY->Get<Transform>(m_currentEntity);
							transform.m_editorRotation = RadiansVectorToDegrees(transform.m_rotation.ToEuler());
						}
					}				
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
}
void ImguiHandler::SaveSceneAs()
{
}
