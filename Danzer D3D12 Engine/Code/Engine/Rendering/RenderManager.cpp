#include "stdafx.h"

#include "../3rdParty/imgui-master/backends/imgui_impl_dx12.h"

#include "RenderManager.h"
#include "Core/DirectX12Framework.h"
#include "Core/MathDefinitions.h"
#include "Core/WindowHandler.h"
#include "Models/ModelHandler.h"
#include "2D/SpriteHandler.h"


#include "Components/Sprite.h"
#include "Components/Text.h"
#include "Components/Object.h"
#include "Components/Transform.h"
#include "Components/Transform2D.h"
#include "Components/DirectionalLight.h"

#include "ScreenTextures/GBuffer.h"
#include "Camera.h"

#include "SkyBox.h"
#include "Scene.h"

#include <algorithm>
#include <type_traits>

class RenderManager::Impl {
public:
	Impl(DirectX12Framework& framework);
	~Impl();

	DirectX12Framework& GetFramework() { return m_framework; }
	PipelineStateHandler& GetPipelineStateHandler() { return m_pipeLineHandler; }

	void RenderImgui();

	void WaitForPreviousFrame();
	void RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler,
		SpriteHandler& SpriteHandler, Skybox& skybox, Scene& scene/*Camera, Ligthing, GameObjects, etc...*/);

private:
	void RenderScene(TextureHandler& textureHandler, ModelHandler& modelHandler, Scene& scene, Skybox& skybox);

	void Update3DInstances(Scene& scene, ModelHandler& modelHandler);
	void Update2DInstances(Scene& scene, SpriteHandler& spriteHandler);

	void AddSpriteSheetInstance(Sprite& data, Transform2D& transform, SpriteHandler& spriteHandler);
	void AddFontInstance(Text& data, Transform2D& trransform, SpriteHandler& spriteHandler);

	void ClearAllInstances(ModelHandler& modelHandler, SpriteHandler& spriteHandler);

	PipelineStateHandler m_pipeLineHandler;
	Renderer			 m_mainRenderer;
	Renderer2D			 m_2dRenderer;
	GBuffer				 m_gBuffer;

	DirectX12Framework& m_framework;

	std::vector<Object*>			      m_transparentObjects;
	std::vector<AABBBuffer::AABBInstance> m_aabbInstances;
	std::vector<RayBuffer::RayInstance>   m_rayInstances;
};


RenderManager::Impl::Impl::Impl(DirectX12Framework& framework) : 
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
	D3D12_RECT rect;
	rect = m_framework.m_scissorRect;
	rect.bottom = WindowHandler::GetWindowData().m_height;
	rect.right  = WindowHandler::GetWindowData().m_width;

	D3D12_VIEWPORT view = m_framework.m_viewport;
	view.Height = (float)WindowHandler::GetWindowData().m_height;
	view.Width  = (float)WindowHandler::GetWindowData().m_width;

	m_framework.m_commandList->RSSetViewports(1, &view);
	m_framework.m_commandList->RSSetScissorRects(1, &rect);

	ID3D12DescriptorHeap* descHeaps[] = { m_framework.GetImguiHeap() };
	m_framework.GetCommandList()->SetDescriptorHeaps(
		_countof(descHeaps),
		&descHeaps[0]
	);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_framework.GetCommandList());
}

void RenderManager::Impl::Impl::WaitForPreviousFrame()
{
	m_framework.WaitForPreviousFrame();
}

void RenderManager::Impl::Impl::RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler,
	SpriteHandler& spriteHandler, Skybox& skybox, Scene& scene)
{	
	ImGui::Render();

	Camera&    cam			= scene.Registry().get<Camera>(scene.GetMainCamera());
	Transform& camTransform = scene.Registry().get<Transform>(scene.GetMainCamera());

	m_framework.ResetCommandListAndAllocator(nullptr);

	m_framework.m_commandList->RSSetViewports(1, &m_framework.m_viewport);
	m_framework.m_commandList->RSSetScissorRects(1, &m_framework.m_scissorRect);
	m_framework.m_commandList->ClearDepthStencilView(m_framework.m_depthDescriptor->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 1, &m_framework.m_scissorRect);

	RenderScene(textureHandler, modelHandler, scene, skybox);
	
	// Transisition resource barrier from present to render target
	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
	//	m_framework.m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
	//	m_framework.m_frameIndex, 
	//	m_framework.m_rtvDescripterSize);
	//CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_framework.m_depthDescriptor->GetCPUDescriptorHandleForHeapStart());
	//m_framework.TransitionRenderTarget(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//m_framework.m_commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	////m_framework.m_commandList-()
	//
	//Vect4f clearColor = { 0.5f, 0.5f, 1.f, 1.f };
	//
	//// Start Model Rendering
	//{
	//	std::vector<ModelData>& models = modelHandler.GetAllModels();
	//	m_framework.m_commandList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_DEFAULT));
	//	m_mainRenderer.UpdateDefaultBuffers(cam, camTransform, m_framework.GetFrameIndex());
	//	
	//	m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_SKYBOX));
	//	m_mainRenderer.RenderSkybox(camTransform, 
	//		textureHandler.GetTextures()[skybox.GetCurrentActiveSkybox()[1] - 1], 
	//		textureHandler.GetTextures()[0],
	//		models[skybox.GetCurrentActiveSkybox()[0] - 1], 
	//		skybox,
	//		m_framework.GetFrameIndex()
	//	);
	//
	//	Update3DInstances(scene, modelHandler);	
	//	m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_MODELS));
	//	m_mainRenderer.DefaultRender(models, m_framework.GetFrameIndex(), textureHandler.GetTextures());
	//	
	//	// * Transparent rendering
	//	//m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_TRANSPARENT));
	//	//m_mainRenderer.TransparentRender(scene, m_transparentObjects, models, m_framework.GetFrameIndex(), textureHandler.GetTextures());
	//}
	//// End Model Renedering
	//
	//
	//// Render editor models
	//if (s_engineState == EngineState::ENGINE_STATE_EDITOR) {
	//
	//	// Start Collision Rendering
	//	{
	//		if (!m_aabbInstances.empty()) {
	//			m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_AABB_WIREFRAME));
	//			m_mainRenderer.AABBRendering(m_aabbInstances, m_framework.GetFrameIndex());
	//		}
	//
	//		if (!m_rayInstances.empty()) {
	//			m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_RAY_WIREFRAME));
	//			m_mainRenderer.RayRendering(m_rayInstances, m_framework.GetFrameIndex());
	//		}
	//	}
	//	// End Collision Rendering
	//}
	//
	//// Start UI Rendering
	//{
	//	Update2DInstances(scene, spriteHandler);
	//	
	//	m_framework.m_commandList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_UI)); 
	//	m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_UI));
	//
	//	m_2dRenderer.UpdateDefaultUIBuffers(m_framework.GetFrameIndex());
	//	m_2dRenderer.RenderUI(spriteHandler.GetAllSprites(), m_framework.GetFrameIndex(), textureHandler.GetTextures());
	//
	//	m_framework.m_commandList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_FONT));
	//	m_2dRenderer.RenderFontUI(spriteHandler.GetAllFonts(), m_framework.GetFrameIndex(), textureHandler.GetTextures());
	//}
	//// End UI Rendering 
	//
	//
	//m_framework.TransitionRenderTarget(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	RenderImgui();
	m_framework.ExecuteCommandList();
	ClearAllInstances(modelHandler, spriteHandler);
}

//* Can not be associate to a class and is only used for std::sort funnction! 
bool ObjectFurtherFromCamera(const Object& o1, const Object& o2)
{
	//if (o1.GetDistanceFromCam() > o2.GetDistanceFromCam())
	//	return true;
	
	return false;
}

void RenderManager::Impl::RenderScene(TextureHandler& textureHandler, ModelHandler& modelHandler, Scene& scene, Skybox& skybox)
{
	ID3D12GraphicsCommandList* cmdList = m_framework.GetCommandList();

	entt::registry& reg = scene.Registry();
	Camera& cam = reg.get<Camera>(scene.GetMainCamera());
	Transform& camTransform = reg.get<Transform>(scene.GetMainCamera());
	
	//* Scene to Gbuffer start
	{
		Update3DInstances(scene, modelHandler);
		
		cmdList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_GBUFFER));
		cmdList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_GBUFFER));
		
		m_framework.TransitionMultipleRTV(
			&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		
		m_gBuffer.ClearRenderTargets(cmdList, {0.5f, 0.5f, 1.f, 1.f}, 1, &m_framework.m_scissorRect);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_framework.m_depthDescriptor->GetCPUDescriptorHandleForHeapStart());
		std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> rtvHandle = m_gBuffer.GetRTVDescriptorHandles();
		cmdList->OMSetRenderTargets(GBUFFER_COUNT, &rtvHandle[0], false, &dsvHandle);
		
		m_mainRenderer.UpdateDefaultBuffers(cam, camTransform, m_framework.GetFrameIndex());
		m_mainRenderer.RenderToGbuffer(
			modelHandler.GetAllModels(), 
			m_framework.GetFrameIndex(), 
			textureHandler.GetTextures()
		);	
	
		m_framework.TransitionMultipleRTV(
			&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

	}
	//* Scene to Gbuffer end
	//m_framework.ExecuteCommandList();
	//m_framework.WaitForPreviousFrame();
	//m_framework.ResetCommandListAndAllocator(m_pipeLineHandler.GetPSO(PIPELINE_STATE_GBUFFER));

	//* Render scene Ligthing start
	{
		cmdList->SetGraphicsRootSignature(m_pipeLineHandler.GetRootSignature(ROOTSIGNATURE_STATE_LIGHT));
		cmdList->SetPipelineState(m_pipeLineHandler.GetPSO(PIPELINE_STATE_DIRECTIONAL_LIGHT));
		
		m_framework.TransitionRenderTarget(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		float c[4] = { 0.5f, 0.5f, 1.f, 1.f };
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_framework.GetCurrentRTVHandle();
		cmdList->ClearRenderTargetView(rtvHandle, &c[0], 1, &m_framework.m_scissorRect);
		cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
			
		m_gBuffer.AssignSRVSlots(cmdList, 2);
	
		auto list = scene.Registry().view<DirectionalLight, Transform>();
		DirectionalLight directionalLight;
		Vect4f directionaLightdir = {0.f, 0.f, 0.f, 1.f};
		for (auto entity : list) {
			directionalLight = reg.get<DirectionalLight>(entity);
			Vect3f dir = reg.get<Transform>(entity).GetWorld().Down();
			directionaLightdir = { dir.x, dir.y, dir.z, 1.f };
			break;
		}
	
		m_mainRenderer.UpdateDefaultBuffers(cam, camTransform, m_framework.GetFrameIndex());
		m_mainRenderer.RenderDirectionalLight(
			directionalLight, 
			directionaLightdir, 
			textureHandler.GetTextures()[skybox.GetCurrentActiveSkybox()[1] - 1], 
			m_framework.GetFrameIndex()
		);
			 
		m_framework.TransitionMultipleRTV(
			&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT
		);
	
		m_framework.TransitionRenderTarget(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}
	//* Render scene Ligthing end
	
	m_framework.ExecuteCommandList();
	m_framework.WaitForPreviousFrame();
	m_framework.ResetCommandListAndAllocator(m_pipeLineHandler.GetPSO(PIPELINE_STATE_DIRECTIONAL_LIGHT));
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
					modelData.AddInstanceTransform(transform.m_world); 
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
	

	std::sort(m_transparentObjects.begin(), m_transparentObjects.end(), [](Object* o1, Object* o2) {
		return ObjectFurtherFromCamera(*o1, *o2);
	});
}
void RenderManager::Impl::Update2DInstances(Scene& scene, SpriteHandler& spriteHandler)
{

	entt::registry& reg = scene.Registry();

	auto spriteView = reg.view<Transform2D, Object, Sprite>();
	for (auto entity : spriteView) {
		Object& obj = reg.get<Object>(entity);
		if(obj.m_state == Object::STATE::ACTIVE){
			Sprite& sprite = reg.get<Sprite>(entity);
			if (sprite.m_spriteSheet != 0) {
				AddSpriteSheetInstance(sprite, reg.get<Transform2D>(entity), spriteHandler);
			}
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
	float w = (float)WindowHandler::GetWindowData().m_width / 2.f;
	float h = (float)WindowHandler::GetWindowData().m_height / 2.f;

	SpriteData::Sheet& sheet = spriteHandler.GetAllSprites()[data.m_spriteSheet - 1].GetSheet();
	UINT frame = data.m_frame;
	
	SpriteData::Instance inst;
	inst.m_position	   = sheet.m_frames[frame].m_framePosition;	
	inst.m_sheetSize   = { (float)sheet.m_width, (float)sheet.m_height };
	inst.m_anchor	   = transform.m_position + data.m_pixelOffset * Vect2f(2.f / w, 2.f / h);
	inst.m_frameSize.x = (float)sheet.m_frames[frame].m_width;
	inst.m_frameSize.y = (float)sheet.m_frames[frame].m_height;
	inst.m_uiSize	   = transform.m_scale;

	spriteHandler.GetAllSprites()[data.m_spriteSheet - 1].AddInstance(inst);
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
	m_transparentObjects.clear();
	m_rayInstances.clear();
}

RenderManager::RenderManager(DirectX12Framework& framework)
{
	m_Impl = new Impl(framework);
}

RenderManager::~RenderManager()
{
	m_Impl->~Impl();
}

void RenderManager::RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler, SpriteHandler& SpriteHandler, Skybox& skybox, Scene& scene)
{
	m_Impl->RenderFrame(textureHandler, modelHandler, SpriteHandler, skybox, scene);
}
