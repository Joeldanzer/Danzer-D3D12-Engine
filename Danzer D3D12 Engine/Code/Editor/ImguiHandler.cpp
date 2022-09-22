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

#include "Components/Text.h"
#include "Components/Object.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"

#include "../3rdParty/imgui-master/imgui.h"

ImguiHandler::ImguiHandler() : 
	m_engine(nullptr),
	m_object(nullptr),
	m_object2D(nullptr)
{}
ImguiHandler::~ImguiHandler()
{
	m_object = nullptr;
	m_engine = nullptr;
}

void ImguiHandler::Init(Engine& engine)
{
	m_engine = &engine;
}

void ImguiHandler::Update(const float dt)
{
	//Scene& scene = m_engine->GetSceneManager().GetCurrentScene();
	DirectX12Framework& framework = m_engine->GetFramework();
	Scene& scene = m_engine->GetSceneManager().GetCurrentScene();
	entt::registry& reg = scene.Registry();


	if (ImGui::Begin("Directional Lighting")) {

		auto list = reg.view<DirectionalLight>();
		DirectionalLight* light = nullptr;
		Transform*	  transform = nullptr;
		for (auto ent : list) {
			// Only exist one in every scene
			light = &reg.get<DirectionalLight>(ent);
			transform = &reg.get<Transform>(ent);
			break;
		}

		if (light && transform) {
			static float lightColor[3] = {light->m_lightColor.x, light->m_lightColor.y, light->m_lightColor.z};
			ImGui::ColorEdit3("Light Color", lightColor);
			
			static float lightStr = light->m_lightColor.w;
			ImGui::DragFloat("Light Strength", &lightStr, 0.1f, 0.f, 1000.f);
			light->m_lightColor.w = lightStr;
			
			light->m_lightColor = { lightColor[0], lightColor[1], lightColor[2], lightStr };

			static float ambientColor[4] = { light->m_ambientColor.x, light->m_ambientColor.y, light->m_ambientColor.z, light->m_ambientColor.w};
			ImGui::ColorEdit3("Ambient Color", ambientColor);
			
			static float ambientStr = light->m_ambientColor.w;
			ImGui::DragFloat("AAmbient Strength", &ambientStr, 0.1f, 0.f, 1000.f);
			light->m_ambientColor.w = ambientStr;

			light->m_ambientColor = {ambientColor[0], ambientColor[1], ambientColor[2], ambientStr};

			static float rotation[3] = { transform->m_rotation.x, transform->m_rotation.y, transform->m_rotation.z };
			ImGui::DragFloat3("Rotation", rotation, 0.1f, -1000.f, 1000.f);
			transform->m_rotation = Quat4f::CreateFromYawPitchRoll({ rotation[0], rotation[1], rotation[2] });
		}

		ImGui::End();
	}

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

CD3DX12_GPU_DESCRIPTOR_HANDLE ImguiHandler::AddImguiImage(std::wstring path)
{
	
	for (UINT i = 0; i < m_imguiTextures.size(); i++)
	{
		if (path == m_imguiTextures[i].m_imagePath)
			return m_imguiTextures[i].m_srvGpuHandle;
	}

	m_engine->GetFramework().ResetCommandListAndAllocator(nullptr);
	ImguiTexture texture;

	ID3D12Device* device = m_engine->GetFramework().GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_engine->GetFramework().GetCommandList();

	CD3DX12_RESOURCE_BARRIER barrier = LoadATextures(
		path,
		device,
		cmdList,
		&texture.m_texture
	);

	cmdList->ResourceBarrier(1, &barrier);
	m_engine->GetFramework().ExecuteCommandList();
	m_engine->GetFramework().WaitForPreviousFrame();

	ID3D12DescriptorHeap* imguiDesc = m_engine->GetFramework().GetImguiHeap();

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

void ImguiHandler::Object3DImgui()
{
	TextureHandler& textureHandler = m_engine->GetTextureHandler();
	ModelHandler& modelHandler = m_engine->GetModelHandler();

	//if (m_object) {
	//	ImGui::Text(m_object->GetName().c_str());
	//	ImGui::Separator();
	//	if (ImGui::CollapsingHeader("Transform")) {
	//		m_pos[0] = m_object->GetPosition().x; m_pos[1] = m_object->GetPosition().y; m_pos[2] = m_object->GetPosition().z;
	//		ImGui::DragFloat3("Position", m_pos);
	//		m_object->SetPosition({ m_pos[0], m_pos[1], m_pos[2] });
	//
	//		m_sca[0] = m_object->GetScale().x; m_sca[1] = m_object->GetScale().y; m_sca[2] = m_object->GetScale().z;
	//		ImGui::DragFloat3("Scale", m_sca);
	//		m_object->SetScale({ m_sca[0], m_sca[1], m_sca[2] });
	//
	//		m_rot[0] = m_object->GetRotation().x; m_rot[1] = m_object->GetRotation().y; m_rot[2] = m_object->GetRotation().z;
	//		ImGui::DragFloat3("Rotation", m_rot);
	//		//m_object->SetRotation({ m_rot[0], m_rot[1], m_rot[2] });
	//	}
	//
	//	if (ImGui::CollapsingHeader("Model")) {
	//		if (m_object->GetModel() != 0) {
	//			ModelData& model = modelHandler.GetLoadedModelInformation(m_object->GetModel());
	//
	//			std::string modelNameInfo("Name: " + model.Name());
	//			std::string modelId("ID: " + std::to_string(model.GetID()));
	//			ImGui::Text(modelId.c_str());
	//
	//			ImGui::Text(modelNameInfo.c_str());
	//			ImGui::SliderInt("Mesh", &m_currentMesh, 0, static_cast<int>(model.GetMeshes().size()) - 1);
	//			if (m_currentMesh > model.GetMeshes().size() - 1)
	//				m_currentMesh = 0;
	//
	//			ModelData::Mesh& mesh = model.GetSingleMesh(m_currentMesh);
	//			ImGui::Checkbox("Render mesh", &mesh.m_renderMesh);
	//			ImGui::Separator();
	//			ImGui::Text("Mesh Textures:");
	//			for (UINT i = 0; i < model.GetMeshes().size(); i++)
	//			{
	//				CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(textureHandler.GetTextures()[mesh.m_texture - 1].m_texturePath);
	//				ImGui::Image(
	//					(ImTextureID)srvHandle.ptr,
	//					{ 100, 100 }
	//				);
	//				ImGui::SameLine();
	//			}
	//
	//			// Just to use up the SameLine function after the for loop
	//			ImGui::Text("");
	//			ImGui::Separator();
	//		}
	//	}
	//}
}

void ImguiHandler::Object2DImgui()
{
	TextureHandler& textureHandler = m_engine->GetTextureHandler();

	//if (m_object2D) {
	//	ImGui::Text(m_object2D->GetName().c_str());
	//	ImGui::Separator();
	//	if (ImGui::CollapsingHeader("Transform")) {
	//		m_pos2D[0] = m_object2D->GetPosition().x; m_pos2D[1] = m_object2D->GetPosition().y; 
	//		ImGui::DragFloat3("Position", m_pos2D);
	//		m_object2D->SetPosition({ m_pos2D[0], m_pos2D[1] });
	//
	//		m_rot2D[0] = m_object2D->GetRotation().x; m_rot2D[1] = m_object2D->GetRotation().y; 
	//		ImGui::DragFloat3("Rotation", m_rot2D);
	//		m_object2D->SetRotation({ m_rot2D[0], m_rot2D[1] });
	//	}
	//
	//	for (UINT i = 0; i < m_object2D->GetComponents().size(); i++)
	//	{
	//		Component* comp = m_object2D->GetComponents()[i];
	//
	//		SpriteData* sprite = dynamic_cast<SpriteData*>(comp);
	//		if (sprite) {
	//			if (ImGui::CollapsingHeader("Sprite")) {
	//				Sprite& const spriteSheet = m_engine->GetSpriteFactory().GetSprite(sprite->m_spriteSheet);
	//
	//				std::string spriteName("Name: " + spriteSheet.Name());
	//				std::string spriteID("ID:" + std::to_string(spriteSheet.GetID()));
	//
	//				ImGui::Text(spriteID.c_str());
	//				ImGui::Text(spriteName.c_str());
	//
	//				m_currentFrame = sprite->m_frame;
	//
	//				ImGui::SliderInt("Frame", &m_currentFrame, 0, static_cast<int>(spriteSheet.GetSheet().m_frames.size()) - 1);
	//				sprite->m_frame = m_currentFrame;
	//
	//				ImGui::Separator();
	//				ImGui::Text("Sprite Sheet Texture");
	//				CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(textureHandler.GetTextures()[spriteSheet.GetSheet().m_texture].m_texturePath);
	//				ImGui::Image((ImTextureID)srvHandle.ptr, { 100, 100 });
	//				ImGui::Separator();
	//			}
	//		}
	//		
	//		TextData* text = dynamic_cast<TextData*>(comp);
	//		if (text) {
	//			if (ImGui::CollapsingHeader("Text")) {
	//				Font& const font = m_engine->GetSpriteFactory().GetFont(text->m_font);
	//
	//				std::string fontName("Name: " + font.Name());
	//				std::string fontID("ID:" + std::to_string(font.GetID()));
	//
	//				ImGui::Text(fontID.c_str());
	//				ImGui::Text(fontName.c_str());
	//
	//				strncpy_s(m_text, text->m_text.c_str(), sizeof(char) * _countof(m_text));
	//
	//				ImGui::InputText("Text", m_text, sizeof(char) * _countof(m_text));
	//				text->m_text = std::string(m_text);
	//
	//				ImGui::Separator();
	//				ImGui::Text("Sprite Sheet Texture");
	//				CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle = AddImguiImage(textureHandler.GetTextures()[font.GetData().m_texture].m_texturePath);
	//				ImGui::Image((ImTextureID)srvHandle.ptr, { 100, 100 });
	//				ImGui::Separator();
	//			}
	//		}
	//	}
	//}
}
