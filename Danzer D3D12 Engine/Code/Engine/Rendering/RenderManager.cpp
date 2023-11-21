#include "stdafx.h"

#include "../3rdParty/imgui-master/backends/imgui_impl_dx12.h"
#include "../3rdParty/imgui-master/backends/imgui_impl_win32.h"

#include "RenderManager.h"
#include "Core/D3D12Framework.h"
#include "Core/MathDefinitions.h"
#include "Core/WindowHandler.h"
#include "Models/ModelHandler.h"
#include "2D/SpriteHandler.h"
#include "FrameResource.h"

#include "Components/Sprite.h"
#include "Components/Text.h"
#include "Components/Object.h"
#include "Components/Transform.h"
#include "Components/Transform2D.h"
#include "Components/DirectionalLight.h"


#include "Screen Rendering/GBuffer.h"
#include "Camera.h"

#include "SkyBox.h"
#include "Scene.h"

#include <queue>
#include <algorithm>
#include <type_traits>


class RenderManager::Impl {
public:
	Impl(D3D12Framework& framework);
	~Impl();

	D3D12Framework& GetFramework() { return m_framework; }
	PipelineStateHandler& GetPipelineStateHandler() { return m_pipeLineHandler; }

	void RenderImgui();

	void BeginFrame();
	void RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler,
		SpriteHandler& SpriteHandler, Skybox& skybox, Scene& scene/*Camera, Ligthing, GameObjects, etc...*/);

private:
	void RenderScene(TextureHandler& textureHandler, SpriteHandler& spriteHandler, ModelHandler& modelHandler, Scene& scene, Skybox& skybox);

	void Update3DInstances(Scene& scene, ModelHandler& modelHandler);
	void Update2DInstances(Scene& scene, SpriteHandler& spriteHandler);

	void AddSpriteSheetInstance(Sprite& data, Transform2D& transform, SpriteHandler& spriteHandler);
	void AddFontInstance(Text& data, Transform2D& trransform, SpriteHandler& spriteHandler);

	void ClearAllInstances(ModelHandler& modelHandler, SpriteHandler& spriteHandler);

	PipelineStateHandler m_pipeLineHandler;
	Renderer			 m_mainRenderer;
	Renderer2D			 m_2dRenderer;
	GBuffer				 m_gBuffer;

	D3D12Framework& m_framework;


	//std::priority_queue<TransparentObject, std::vector<TransparentObject>, >
	//struct TransparentObject {
	//	Transform m_transform;
	//	UINT      m_model;
	//};

	//std::vector<ModelData*>			      m_transparentObjects;
	std::vector<AABBBuffer::AABBInstance> m_aabbInstances;
	std::vector<RayBuffer::RayInstance>   m_rayInstances;
};


RenderManager::Impl::Impl::Impl(D3D12Framework& framework) : 
	m_framework(framework),
	m_gBuffer(framework)
{
	m_2dRenderer.Init(framework);
	m_mainRenderer.Init(framework);
	m_pipeLineHandler.Init(framework);
}
RenderManager::Impl::Impl::~Impl(){

	m_mainRenderer.~Renderer();
	m_pipeLineHandler.~PipelineStateHandler();
}

void RenderManager::Impl::Impl::RenderImgui()
{
	ID3D12DescriptorHeap* descHeaps[] = { m_framework.m_imguiDesc };
	m_framework.m_frameResources[m_framework.m_frameIndex]->CmdList()->SetDescriptorHeaps(
		_countof(descHeaps),
		&descHeaps[0]
	);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_framework.m_frameResources[m_framework.m_frameIndex]->CmdList());
}

void RenderManager::Impl::BeginFrame()
{
	ImGui::NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	m_framework.InitiateCommandList(nullptr, L"RenderManager Line " + __LINE__);
	
	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	cmdList->RSSetViewports(1, &m_framework.m_viewport);
	cmdList->RSSetScissorRects(1, &m_framework.m_scissorRect);
	cmdList->ClearDepthStencilView(
		m_framework.DSVHeap().GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &m_framework.m_scissorRect
	);
	
	m_framework.QeueuResourceTransition(&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT, 
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ID3D12Resource* resources[] = { m_framework.GetCurrentRTV() };
	m_framework.QeueuResourceTransition(&resources[0], 1,
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	m_framework.TransitionAllResources();

	// Clear all RenderTargets
	m_gBuffer.ClearRenderTargets(m_framework.RTVHeap(), cmdList, 1, &m_framework.m_scissorRect);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_framework.RTVHeap().GET_CPU_DESCRIPTOR(0);
	rtvHandle.Offset(m_framework.m_frameIndex * m_framework.RTVHeap().DESCRIPTOR_SIZE());
	cmdList->ClearRenderTargetView(
		rtvHandle,
		&ClearColor[0], 
		1,
		&m_framework.m_scissorRect
	);

}

// DirectX12Framework pipeline needs to be fully reworked it seems >:(

void RenderManager::Impl::Impl::RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler,
	 SpriteHandler& spriteHandler, Skybox& skybox, Scene& scene)
{	
	ImGui::Render();
	RenderScene(textureHandler, spriteHandler, modelHandler, scene, skybox);

	ClearAllInstances(modelHandler, spriteHandler);
}

void RenderManager::Impl::RenderScene(TextureHandler& textureHandler, SpriteHandler& spriteHandler, ModelHandler& modelHandler, Scene& scene, Skybox& skybox)
{
	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	ID3D12DescriptorHeap* cbvSrvHeap = m_framework.CbvSrvHeap().GetDescriptorHeap();

	const UINT frameIndex = m_framework.m_frameIndex;

	entt::registry& reg = scene.Registry();
	Camera& cam = reg.get<Camera>(scene.GetMainCamera());
	Transform& camTransform = reg.get<Transform>(scene.GetMainCamera());

	{ //* Scene to Gbuffer start
		Update3DInstances(scene, modelHandler);


		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_framework.DSVHeap().GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
		std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> rtvHandle = m_gBuffer.GetRTVDescriptorHandles(m_framework.RTVHeap());
		cmdList->OMSetRenderTargets(GBUFFER_COUNT, &rtvHandle[0], false, &dsvHandle);
		
		cmdList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_GBUFFER));
		cmdList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_GBUFFER));
		cmdList->SetDescriptorHeaps(1, &cbvSrvHeap);

		UINT startLocation = 0;

		CD3DX12_GPU_DESCRIPTOR_HANDLE defaultHandle = m_mainRenderer.UpdateDefaultBuffers(cam, camTransform, m_framework.GetFrameIndex());
		cmdList->SetGraphicsRootDescriptorTable(startLocation, defaultHandle);
		startLocation++;

		// Render non transparent objects first
		m_mainRenderer.RenderToGbuffer(
			cmdList,
			modelHandler.GetAllModels(),
			m_framework.m_frameIndex,
			textureHandler.GetTextures(),
			false,
			startLocation
		);

		m_framework.QeueuResourceTransition(
			&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_framework.TransitionAllResources();
	} //* Scene to Gbuffer end

	{ //* Render Skybox & Ligth start
		cmdList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_DEFAULT));
		cmdList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_SKYBOX));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_framework.m_rtvHeap.GET_CPU_DESCRIPTOR(0);
		rtvHandle.Offset(frameIndex * m_framework.RTVHeap().DESCRIPTOR_SIZE());

		cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//ID3D12DescriptorHeap* cbvSrvHeap = m_framework.GetCbvSrvUavWrapper().GetDescriptorHeap();
		//m_framework.m_commandList->SetDescriptorHeaps(1, &cbvSrvHeap);

		UINT startLocation = 0;
		
		D3D12_GPU_DESCRIPTOR_HANDLE defaultHandle = m_mainRenderer.UpdateDefaultBuffers(cam, camTransform, m_framework.GetFrameIndex());
		cmdList->SetGraphicsRootDescriptorTable(startLocation, defaultHandle);
		startLocation++;

		m_mainRenderer.RenderSkybox(
			cmdList,
			camTransform,
			textureHandler.GetTextures()[skybox.GetCurrentSkyboxTexture() - 1],
			modelHandler.GetAllModels()[skybox.GetSkyboxCube() - 1],
			skybox,
			m_framework.GetFrameIndex(),
			startLocation
		);

		cmdList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_LIGHT));
		cmdList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_DIRECTIONAL_LIGHT));

		auto list = scene.Registry().view<DirectionalLight, Transform>();
		DirectionalLight directionalLight;
		Vect4f directionaLightdir = { 0.f, 0.f, 0.f, 1.f };
		for (auto entity : list) {
			directionalLight = reg.get<DirectionalLight>(entity);
			Transform& transform = reg.get<Transform>(entity);
			Vect3f dir = transform.m_world.Down();
			directionaLightdir = { dir.x, dir.y, dir.z, 1.0f };
		}

		D3D12_GPU_DESCRIPTOR_HANDLE lightHandle = m_mainRenderer.UpdateLightBuffer(directionalLight, directionaLightdir, m_framework.GetFrameIndex());
		cmdList->SetGraphicsRootDescriptorTable(startLocation, lightHandle);
		startLocation++;

		m_gBuffer.AssignSRVSlots(cmdList, &m_framework.CbvSrvHeap(), startLocation);
		m_mainRenderer.RenderDirectionalLight(
			cmdList,
			textureHandler.GetTextures()[skybox.GetCurrentActiveSkybox()[1] - 1],
			m_framework.GetFrameIndex(),
			startLocation
		);
	} //* Render scene Ligthing end

	{ //* Render 2D Scene
		cmdList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_UI));
		cmdList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_UI));

		ID3D12DescriptorHeap* cbvSrvHeap = m_framework.CbvSrvHeap().GetDescriptorHeap();
		cmdList->SetDescriptorHeaps(1, &cbvSrvHeap);

		Update2DInstances(scene, spriteHandler);
		m_2dRenderer.RenderUI(
			cmdList, 
			spriteHandler.GetAllSprites(), 
			m_framework.GetFrameIndex(), 
			textureHandler.GetTextures()
		);
	} //* Render 2D Scene End

	RenderImgui(); // Render Imgui over everything else

	m_framework.TransitionRenderTarget(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void RenderManager::Impl::Update3DInstances(Scene& scene, ModelHandler& modelHandler)
{

	entt::registry& reg = scene.Registry();
	Transform& cam = reg.get<Transform>(scene.GetMainCamera());

	auto view = reg.view<Transform, Model, Object>();

	for (auto entity : view)
	{
		Object& obj = reg.get<Object>(entity);

		if (obj.m_state == Object::STATE::ACTIVE) {
			Model& model = reg.get<Model>(entity);
			if (model.m_modelID != 0) {
				Transform& transform = reg.get<Transform>(entity);
				ModelData& modelData = modelHandler.GetLoadedModelInformation(model.m_modelID);

				if (modelData.IsTransparent()) {
					//Mat4f old = transform.GetWorld();
					//obj->Translate(old); //* scene->GetLayer(obj->GetLayer()).m_layerTransform);
					//obj->SetDistanceFromCam(cam.GetPosition(), cam.GetTransform().Forward());// * m_camera->GetTransform().Forward());
					//obj->Translate(old);
					//m_transparentObjects.emplace_back(obj);
				}
				else {
					modelData.AddInstanceTransform(DirectX::XMMatrixTranspose(transform.m_world)); 
				}
			}
		}	
	}	
	//* Collider code i want to keep 
	//if (s_engineState == EngineState::ENGINE_STATE_EDITOR) {
	//	if (obj->GetComponent("AABBCollider")) {
	//		AABBCollider* col = static_cast<AABBCollider*>(obj->GetComponent("AABBCollider"));
	//		if (col->m_active) {
	//			AABBCollider::AABBInstance instance;
	//			instance.m_position = col->GetPosition();
	//			instance.m_size = col->GetSize();
	//			m_aabbInstances.emplace_back(instance);
	//		}
	//	}
	//
	//	if (obj->GetComponent("RayCollider")) {
	//		RayCollider* col = static_cast<RayCollider*>(obj->GetComponent("RayCollider"));
	//		if (col->m_active) {
	//			RayCollider::RayInstance instance;
	//			instance.m_position    = col->GetOrigin();
	//			instance.m_destination = col->GetOrigin() + col->GetDirection() * col->GetDistance();	
	//			m_rayInstances.emplace_back(instance);
	//		}
	//	}
	//}

	// Sorts object from closest to furthes for transparent rendering

}
void RenderManager::Impl::Update2DInstances(Scene& scene, SpriteHandler& spriteHandler)
{
	entt::registry& reg = scene.Registry();

	auto spriteView = reg.view<Transform2D, Sprite>();
	for (auto entity : spriteView) {
		Object& obj = reg.get<Object>(entity);
		if(obj.m_state == Object::STATE::ACTIVE){
			Sprite& sprite = reg.get<Sprite>(entity);
			AddSpriteSheetInstance(sprite, reg.get<Transform2D>(entity), spriteHandler);
			//if (sprite.m_spriteSheet != 0) {
			//}
		}
	}

	auto textView = reg.view<Transform2D, Object, Text>();
	for (auto entity : textView) {
		Object& obj = reg.get<Object>(entity);
		if (obj.m_state == Object::STATE::ACTIVE) {
			Text& text = reg.get<Text>(entity);
			if (text.m_fontID != 0) {
				AddFontInstance(text, reg.get<Transform2D>(entity), spriteHandler);
			}
		}
	}
}

void RenderManager::Impl::AddSpriteSheetInstance(Sprite& data, Transform2D& transform, SpriteHandler& spriteHandler)
{
	float w = (float)WindowHandler::WindowData().m_w  / 2.f;
	float h = (float)WindowHandler::WindowData().m_h / 2.f;

	SpriteData::Sheet& sheet = spriteHandler.GetAllSprites()[data.m_spriteSheet].GetSheet();
	UINT frame = data.m_frame;
	
	SpriteData::Instance inst;
	inst.m_position	   = sheet.m_frames[frame].m_framePosition;	
	inst.m_sheetSize   = { (float)sheet.m_width, (float)sheet.m_height };
	inst.m_anchor	   = transform.m_position + data.m_pixelOffset * Vect2f(2.f / w, 2.f / h);
	inst.m_frameSize.x = (float)sheet.m_frames[frame].m_width;
	inst.m_frameSize.y = (float)sheet.m_frames[frame].m_height;
	inst.m_uiSize	   = transform.m_scale;

	spriteHandler.GetAllSprites()[data.m_spriteSheet].AddInstance(inst);
}

void RenderManager::Impl::AddFontInstance(Text& data, Transform2D& transform, SpriteHandler& spriteHandler)
{
	Vect2f newPosition = {0.f, 0.f};	
	std::string& text = data.m_text;

	std::vector<Font>& fonts = spriteHandler.GetAllFonts();

	for (UINT j = 0; j < text.size(); j++)
	{
		char chr = text[j];
		//else {
		Font::Letter letterData = fonts[data.m_fontID - 1].GetData().m_letters[chr];

		Font::Instance inst;

		inst.m_fontPosition = { letterData.m_position.x, letterData.m_position.y, 0.f, 1.f};
		inst.m_fontSize = { (float)letterData.m_width, (float)letterData.m_height };
		inst.m_sheetSize =  {(float)fonts[data.m_fontID - 1].GetData().m_width, (float)fonts[data.m_fontID - 1].GetData().m_height };
		inst.m_position = newPosition;

		inst.m_color =  data.m_color;
		inst.m_size =   transform.m_scale;
		inst.m_anchor = transform.m_position;
	
		newPosition.x += 1.1f;
		fonts[data.m_fontID - 1].AddInstance(inst);
	}
}

void RenderManager::Impl::ClearAllInstances(ModelHandler& modelHandler, SpriteHandler& spriteHandler)
{
	for (auto& model : modelHandler.GetAllModels())
	{
		model.ClearInstanceTransform();
	}

	for (auto& sprite : spriteHandler.GetAllSprites())
	{
		sprite.ClearInstances();
	}

	for (auto& font : spriteHandler.GetAllFonts())
	{
		font.ClearInstances();
	}

	m_aabbInstances.clear();
	//m_transparentObjects.clear();
	m_rayInstances.clear();
}

RenderManager::RenderManager(D3D12Framework& framework)
{
	m_Impl = new Impl(framework);
}

RenderManager::~RenderManager()
{
	m_Impl->~Impl();
}

void RenderManager::BeginFrame()
{
	m_Impl->BeginFrame();
}

void RenderManager::RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler, SpriteHandler& SpriteHandler, Skybox& skybox, Scene& scene)
{
	m_Impl->RenderFrame(textureHandler, modelHandler, SpriteHandler, skybox, scene);
}


