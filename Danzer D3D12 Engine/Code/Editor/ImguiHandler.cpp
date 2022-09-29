#include "ImguiHandler.h"

#include "Core/Engine.h"
#include "Core/WindowHandler.h"
#include "Rendering/RenderManager.h"
#include "Rendering/RenderUtility.h"
#include "Rendering/2D/SpriteHandler.h"
#include "SceneManager.h"
#include "Rendering/Models/ModelData.h"
#include "Core/DirectX12Framework.h"
#include "Rendering/Models/ModelHandler.h"
#include "Components/DirectionalLight.h"
#include "Rendering/TextureHandler.h"

#include "Components/Text.h"
#include "Components/Object.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"

#include <shtypes.h>
#include <ShlObj_core.h>
#include <tchar.h>

#include "../3rdParty/imgui-master/imgui.h"

ImguiHandler::ImguiHandler() : 
	m_engine(nullptr)
{}
ImguiHandler::~ImguiHandler()
{
    m_name = nullptr;
    m_tag  = nullptr;
	m_engine = nullptr;
}

void ImguiHandler::Init(Engine& engine)
{
	m_engine = &engine;
	
	m_rightWindow.m_height = WindowHandler::GetWindowData().m_height;
	m_rightWindow.m_width = 400;
	m_rightWindow.m_positon.x = WindowHandler::GetWindowData().m_width - (m_rightWindow.m_width/2);
	m_rightWindow.m_positon.y = m_rightWindow.m_height / 2;

	m_leftWindow.m_height = WindowHandler::GetWindowData().m_height / 2;
	m_leftWindow.m_width = 400;
	m_leftWindow.m_positon.x = m_leftWindow.m_width / 2;
	m_leftWindow.m_positon.y = m_leftWindow.m_height / 2;

	m_tag = new char;
	m_name = new char;
}

void ImguiHandler::Update(const float dt)
{
	DirectX12Framework& framework = m_engine->Framework();
	Scene& scene = m_engine->SceneManager().GetCurrentScene();
	entt::registry& reg = scene.Registry();

	StaticWindows();
	
	//if (ImGui::ListBoxHeader("Scene View")) {
	//	
	//
	//	ImGui::ListBoxFooter();
	//+}

	//if (ImGui::BeginMainMenuBar()) {
	//
	//	if (ImGui::BeginMenu("File")) {
	//		if(ImGui::MenuItem("File Stuff")){
	//		}
	//
	//		ImGui::EndMenu();
	//	}
	//
	//	ImGui::EndMainMenuBar();
	//}
	//
	//bool show = true;
	//
	//
	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	//
	//unsigned int w = WindowHandler::GetWindowData().m_width;
	//unsigned int h = WindowHandler::GetWindowData().m_height;
	//
	//static bool selected3DItem = false;
	//static bool selected2DItem = false;
	//
	//
	//ImGui::SetNextWindowPos({ 0, 19 });
	//ImGui::SetNextWindowSize({(float)w/4, (float)h/2});
	//
	//if (ImGui::Begin("##", &show, window_flags)) {
	//	
	//	if(ImGui::BeginTabBar("##tabbar1", ImGuiTabBarFlags_None)) {
	//		if (ImGui::BeginTabItem("Scene")) {
	//			
	//			m_selectedScene = 1;
	//
	//			if (ImGui::ListBoxHeader("##Scene Window", { (float)(w/4) - 15, (float)(h/2) - 55 })) {
	//				
	//		
	//				//for (UINT i = 0; i < scene->GetObjects().size(); i++)
	//				//{
	//				//	// WHY WARNING??
	//				//	Object& obj = *scene->GetObjects()[i];
	//				//	if (ImGui::Selectable(obj.GetName().c_str(), selected3DItem)) {
	//				//		obj.Select(true);
	//				//		m_object = &obj;
	//				//	}
	//				//	else {
	//				//		obj.Select(false);
	//				//	}
	//				//}
	//			
	//				ImGui::ListBoxFooter();
	//			}
	//			ImGui::EndTabItem();
	//		}
	//
	//		if (ImGui::BeginTabItem("UI")) {
	//
	//			m_selectedScene = -1;
	//
	//			if (ImGui::ListBoxHeader("##UI Window", { (float)(w / 4) - 15, (float)(h / 2) - 55 })) {
	//
	//
	//				//for (UINT i = 0; i < scene->GetSprites().size(); i++)
	//				//{
	//				//	// WHY WARNING??
	//				//	//Object2D* obj = scene->GetSprites()[i];
	//				//	//if (ImGui::Selectable(obj->GetName().c_str(), selected2DItem)) {
	//				//	//	obj->Select(true);
	//				//	//	m_object2D = obj;
	//				//	//}
	//				//	//else {
	//				//	//	obj->Select(false);
	//				//	//}
	//				//}
	//
	//				ImGui::ListBoxFooter();
	//			}
	//			ImGui::EndTabItem();
	//		}
	//		ImGui::EndTabBar();
	//	}
	//
	//
	//	//ImGui::PopStyleColor();
	//	ImGui::End();
	//}
	//
	//ImGui::SetNextWindowPos({float(w - (w/4)), 19.f});
	//ImGui::SetNextWindowSize({ (float)w / 4, (float)h - 40.f });
	//
	//if (ImGui::Begin("##info", &show, window_flags)) {
	//
	//	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
	//		if (ImGui::BeginTabItem("Object")) {
	//
	//			if(m_selectedScene == 1)
	//				Object3DImgui();
	//
	//			if(m_selectedScene == -1)
	//				Object2DImgui();
	//
	//
	//			ImGui::EndTabItem();
	//		}
	//
	//		ImGui::EndTabBar();
	//	}
	//	ImGui::End();
	//}
	//
	////ImGui::ShowDemoWindow(&show);
}


void ImguiHandler::StaticWindows()
{
	ImGuiWindowFlags staticWindowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	entt::registry& reg = m_engine->SceneManager().GetCurrentScene().Registry();
	//* Left Side window Begin
	{
		ImGui::SetNextWindowSize({ (float)m_leftWindow.m_width, (float)m_leftWindow.m_height });
		ImGui::SetNextWindowPos({ m_leftWindow.m_positon.x, m_leftWindow.m_positon.y }, 0, { 0.5f, 0.5f });
		ImGui::SetNextWindowBgAlpha(1.f);
		bool isOpen = true;

		//static bool selectedEntity = false;

		if (ImGui::Begin("Scene View", &isOpen, staticWindowFlags)) {
			if (ImGui::ListBoxHeader("##", {(float)m_leftWindow.m_width, (float)m_leftWindow.m_width})) {
				auto scene = reg.view<Transform, Object>();
				for (auto entity : scene) {
					Object& obj = reg.get<Object>(entity);
					bool isSelected = (entity == m_currentEntity);
					if (ImGui::Selectable(obj.m_name.c_str(), isSelected)) {
						m_currentEntity = entity;
						m_currentMesh = 0;
						Transform& transform = reg.get<Transform>(entity);
						//memcpy(m_currentRotation, &transform.m_rotation.x, sizeof(float) * 3);

						if (!m_itemsHasBeenSelected)
							m_itemsHasBeenSelected = true;
					}


				}
				
				ImGui::ListBoxFooter();
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
			}
				//Transform& transform = reg.get<Transform>(m_currentEntity);
			ImGui::End();
		}
	}
	//* Right side window en
}
bool ImguiHandler::ModelDataSettings(entt::registry& reg)
{
	Model* model = reg.try_get<Model>(m_currentEntity);
	if (model) {
		if (ImGui::CollapsingHeader("ModelData")) {
			ModelData& data = m_engine->ModelHandler().GetLoadedModelInformation(model->m_modelID);
			
			ImGui::Text("Model Name: ");
			ImGui::SameLine();
			ImGui::Text(data.Name().c_str());

			if (m_currentMesh > data.GetMeshes().size() - 1)
				m_currentMesh = 0;
			
			ImGui::SliderInt("Mesh", &m_currentMesh, 0, data.GetMeshes().size() - 1);
			ModelData::Mesh& currentMesh = data.GetMeshes()[m_currentMesh];

			Material& material = currentMesh.m_material;
			static float roughness = material.m_roughness;
			static float shininess = material.m_shininess;
			static float emissive = material.m_emissvie;
			static float color[3] = { material.m_color[0], material.m_color[1], material.m_color[2] };

			ImGui::ColorEdit3("Albedo Color", color);
			ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.1f,  2.f);
			ImGui::DragFloat("Shininess", &shininess, 0.01f, 0.0f,  2.f);
			ImGui::DragFloat("Emissive",  &emissive,  0.01f, 0.0f,  5.f);

			material.m_roughness = roughness;
			material.m_shininess = shininess;
			material.m_emissvie = emissive;
			memcpy(material.m_color, color, sizeof(float) * 3);
			
			ImGui::Separator();
			ImGui::Text("Textures");

			//Abedo start
			{
				std::wstring albedo;
				if (ImGui::Button("Albedo")) {
					albedo = OpenFileExplorer();
					material.m_albedo = m_engine->TextureHandler().GetTexture(albedo);
					if (material.m_albedo == 0){
						material.m_albedo = m_engine->TextureHandler().CreateTexture(albedo);
					}
				}
				else
					albedo = m_engine->TextureHandler().GetTextureData(material.m_albedo).m_texturePath;

				ImGui::SameLine();
				CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(albedo);
				ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
			}
			//Albedo end

			ImGui::SameLine();
			{
				std::wstring normal;
				if (ImGui::Button("Normal")) {
					normal = OpenFileExplorer();
					material.m_normal = m_engine->TextureHandler().GetTexture(normal);
					if (material.m_normal  == 0) {
						material.m_normal = m_engine->TextureHandler().CreateTexture(normal);
					}
				}
				else
					normal = m_engine->TextureHandler().GetTextureData(material.m_normal).m_texturePath;

				ImGui::SameLine();
				CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(normal);
				ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
			}

			ImGui::SameLine();
			{
				std::wstring metallic;
				if (ImGui::Button("Metallic")) {
					metallic = OpenFileExplorer();
					material.m_metallic = m_engine->TextureHandler().GetTexture(metallic);
					if (material.m_metallic == 0) {
						material.m_metallic = m_engine->TextureHandler().CreateTexture(metallic);
					}
				}
				else
					metallic = m_engine->TextureHandler().GetTextureData(material.m_metallic).m_texturePath;


				ImGui::SameLine();
				CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(metallic);
				ImGui::Image((ImTextureID)srvHandle.ptr, { 50.f, 50.f });
			}

		}
			return true;
	}

	return false;
}

bool ImguiHandler::ObjectSettings(entt::registry& reg)
{	
	Object& obj = reg.get<Object>(m_currentEntity);
	ImGui::Text(obj.m_name.c_str());
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Object")) {

		int len = strlen(obj.m_tag.c_str());
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
						obj.m_state = Object::STATE::ACTIVE;
					}

					if (currentState == m_stateNames[1]) {
						obj.m_state = Object::STATE::NOT_ACTIVE;
					}

					if (currentState == m_stateNames[2]) {
						obj.m_state = Object::STATE::DESTROY;
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
		float rotation[3] = { euler.y, euler.x, euler.z };

		ImGui::DragFloat3("Position", position, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Rotation", rotation, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Scale",    scale,    0.01f, -FLT_MAX, FLT_MAX);

		transform.m_position = { position[0], position[1], position[2], 1.f };
		transform.m_rotation = Quat4f::CreateFromYawPitchRoll(rotation[0], rotation[1], rotation[2]);
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

CD3DX12_GPU_DESCRIPTOR_HANDLE ImguiHandler::AddImguiImage(std::wstring path)
{
	
	for (UINT i = 0; i < m_imguiTextures.size(); i++)
	{
		if (path == m_imguiTextures[i].m_imagePath)
			return m_imguiTextures[i].m_srvGpuHandle;
	}

	m_engine->Framework().ResetCommandListAndAllocator(nullptr);
	ImguiTexture texture;

	ID3D12Device* device = m_engine->Framework().GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_engine->Framework().GetCommandList();

	CD3DX12_RESOURCE_BARRIER barrier = LoadATextures(
		path,
		device,
		cmdList,
		&texture.m_texture
	);

	cmdList->ResourceBarrier(1, &barrier);
	m_engine->Framework().ExecuteCommandList();
	m_engine->Framework().WaitForPreviousFrame();

	ID3D12DescriptorHeap* imguiDesc = m_engine->Framework().GetImguiHeap();

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;

	UINT inrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvGpuHandle.InitOffsetted(imguiDesc->GetGPUDescriptorHandleForHeapStart(), static_cast<INT>(m_imguiTextures.size()) + 1, inrementSize);
	srvCpuHandle.InitOffsetted(imguiDesc->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(m_imguiTextures.size()) + 1, inrementSize);

	DirectX::CreateShaderResourceView(
		device,
		texture.m_texture,
		srvCpuHandle
	);

	texture.m_imagePath = path;
	texture.m_srvGpuHandle = srvGpuHandle;
	m_imguiTextures.emplace_back(texture);


	return m_imguiTextures[m_imguiTextures.size() - 1].m_srvGpuHandle;
}
std::wstring ImguiHandler::OpenFileExplorer()
{
	//LPOPENFILE
	WCHAR fileName[MAX_PATH];//[MAX_PATH];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(&fileName, sizeof(fileName)); 
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T(".dds");
	ofn.lpstrFile = fileName;
	ofn.lpstrInitialDir = _T("Sprites\\");
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = _T("Select Texture");
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn)) {
		
		std::wstring texture(fileName);
		if (!texture.empty() || texture.find_last_of(L".dds") != std::wstring::npos) {
			size_t pos = texture.find(L"Sprites\\");
			texture = texture.erase(0, pos);
			
			pos = texture.find(L"\\");
			texture.replace(pos, 1, L"/");

			return texture;
		}
	}
	return L"";
}
