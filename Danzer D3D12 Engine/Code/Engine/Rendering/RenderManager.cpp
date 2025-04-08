#include "stdafx.h"

#include "Renderer.h"
#include "Rendering/2D/2DRenderer.h"

#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "ImGuizmo/ImGuizmo.h"

#include "PSOHandler.h"
#include "RenderManager.h"
#include "Models/ModelHandler.h"
#include "2D/SpriteHandler.h"
#include "Core/FrameResource.h"
#include "Models/ModelEffectHandler.h"

#include "Components/AllComponents.h"

#include "Screen Rendering/GBuffer.h"
#include "Screen Rendering/Textures/FullscreenTexture.h"
#include "Screen Rendering/TextureRenderer.h"
#include "Screen Rendering/DirectionalShadowMapping.h"
#include "Screen Rendering/Fullscreen Shaders/KuwaharaFilter.h"
#include "Screen Rendering/Textures/TextureRenderingHandler.h"
#include "Screen Rendering/DebugTextureRenderer.h"
#include "Screen Rendering/LightRenderer.h"

#include "Data/LightHandler.h"
#include "Data/VolumetricLightData.h"
#include "Data/SSAOData.h"
#include "Data/DebugRenderingData.h"
#include "Buffers/BufferHandler.h"

#include "Camera.h"
#include "SkyBox.h"
#include "SceneManager.h"

#include <queue>
#include <algorithm>
#include <type_traits>


class RenderManager::Impl {
public:
	Impl(D3D12Framework& framework, TextureHandler& TextureHandler);
	~Impl();

	D3D12Framework& GetFramework() { return m_framework; }

	void InitializeRenderTextures(TextureHandler& textureHandler, ModelHandler& modelHandler);
	void RenderImgui();

	void BeginFrame();
	void RenderFrame(
		TextureHandler& textureHandler, ModelHandler& modelHandler, 
		ModelEffectHandler& effectHandler, SpriteHandler& SpriteHandler, SceneManager& scene/*Camera, Ligthing, GameObjects, etc...*/
	);

	PSOHandler&				 GetPSOHandler();
	TextureRenderingHandler& GetTextureRendering();
	BufferHandler&			 GetConstantBufferHandler();

	void SetKuwaharaRadius(UINT radius, UINT scale, Vect3f offset) {
		m_kuwaharaFilter.SetFilterRadius(radius, scale, offset);
	}

private:
	void RenderScene(TextureHandler& textureHandler, SpriteHandler& spriteHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler, SceneManager& scene);

	void FrustrumCulling(const Camera& camera, Transform& transform, ModelData& model);

	void Update3DInstances(const Camera& cam, SceneManager& scene, ModelHandler& modelHandler, ModelEffectHandler& effectHandler);
	void Update2DInstances(SceneManager& scene, SpriteHandler& spriteHandler);

	void AddSpriteSheetInstance(Sprite& data, Transform2D& transform, SpriteHandler& spriteHandler);
	void AddFontInstance(TextOLD& data, Transform2D& trransform, SpriteHandler& spriteHandler);

	void UpdatePrimaryConstantBuffers(SceneManager& scene);
	void ClearAllInstances(ModelHandler& modelHandler, SpriteHandler& spriteHandler);

	PSOHandler	m_psoHandler;
	Renderer	m_mainRenderer;
	Renderer2D	m_2dRenderer;
	GBuffer		m_gBuffer;
	
	UINT m_lightRootSignture;
	UINT m_dirLightPSO;
	UINT m_pointLightPSO;

	D3D12Framework&           m_framework;
	TextureRenderingHandler   m_textureRendering;
	BufferHandler			  m_bufferHandler;
	LightHandler			  m_lightHandler;

	SSAOData				  m_ssaoData;
	VolumetricLightData       m_vlData;
	DebugRenderingData		  m_debugRenderingData;
				  
	TextureRenderer*          m_dirLightTexture			= nullptr;
							  
	ConstantBufferData*       m_cameraBuffer			= nullptr;
	ConstantBufferData*		  m_shadowMapBuffer		    = nullptr;
	ConstantBufferData*		  m_fullscreenTextureBuffer = nullptr;
	ConstantBufferData*		  m_lightBuffer			    = nullptr;

	Skybox*					  m_skyboxRenderer			= nullptr;
	KuwaharaFilter			  m_kuwaharaFilter;
	DirectionalShadowMapping* m_shadowMap				= nullptr;
	DebugTextureRenderer*     m_debugRenderer			= nullptr;
	LightRenderer*			  m_lightRenderer		    = nullptr;

	struct DefaultBuffer {
		Mat4f  m_transformOne;
		Mat4f  m_transformTwo;
		Vect4f m_vectorOne;
		Vect4f m_vectorTwo;
		Vect2f m_windowSize;
		float  m_time;
	};

	struct LightDefaultBuffer {
		Mat4f  m_lightTransform;
		Mat4f  m_lightProjection;
		Vect4f m_direction;
		Vect4f m_lightColor;
		Vect4f m_ambientColor;
	};

	DefaultBuffer      m_camBufferData;
	LightDefaultBuffer m_lightBufferData;
};


RenderManager::Impl::Impl(D3D12Framework& framework, TextureHandler& textureHandler) :
	m_framework(framework),
	m_psoHandler(framework),
	m_bufferHandler(framework),
	m_textureRendering(framework, m_psoHandler),
	m_gBuffer(framework, m_psoHandler),
	m_debugRenderingData(framework.GetDevice()),
	m_lightHandler(framework),
	m_shadowMap()
{
	// Set the back buffer PSO and RS for use at the end of a frame.
	m_framework.SetBackBufferPSO(m_psoHandler);

	// Setup the main rendering components
	m_2dRenderer.Init(framework, m_psoHandler);
	m_mainRenderer.Init(framework);
}
RenderManager::Impl::~Impl(){

	m_mainRenderer.~Renderer();
	m_psoHandler.~PSOHandler();
}

void RenderManager::Impl::InitializeRenderTextures(TextureHandler& textureHandler, ModelHandler& modelHandler)
{
	m_cameraBuffer    = m_bufferHandler.CreateBufferData(sizeof(DefaultBuffer));
	m_lightBuffer     = m_bufferHandler.CreateBufferData(sizeof(LightBuffer));
	m_shadowMapBuffer = m_bufferHandler.CreateBufferData((sizeof(Mat4f)* 2) + sizeof(Vect4f));

	m_textureRendering.SetCommonBuffers(m_cameraBuffer->OffsetID(), m_lightBuffer->OffsetID());

	m_skyboxRenderer = new Skybox(modelHandler);
	m_skyboxRenderer->Initialize(m_psoHandler, textureHandler);
	m_skyboxRenderer->SetBufferAtSlot(m_cameraBuffer, 0);
	m_textureRendering.AddTextureRendererToPipeline(m_skyboxRenderer, SCENE_PASS_0);

	m_shadowMap = new DirectionalShadowMapping();
	m_shadowMap->SetPipelineAndRootSignature(m_psoHandler);
	m_shadowMap->SetBufferAtSlot(m_shadowMapBuffer->OffsetID(), 0, true);
	m_shadowMap->CreateProjection(128.0f, 8.0f);
	m_textureRendering.AddTextureRendererToPipeline(m_shadowMap, PRE_SCENE_PASS_0);
	FullscreenTexture* shadowMapTexture = m_textureRendering.CreateFullscreenTexture(
		L"Shadow Map Texture",
		8192,
		8192,
		DXGI_FORMAT_R32_FLOAT,
		PRE_SCENE_PASS_0,
		1,
		true
	);
	m_shadowMap->SetDepthStencilView(shadowMapTexture->DSVOffsetID());

	TexturePipelineData rendererData;
	rendererData.m_blendDesc    = PSOHandler::BLEND_DEFAULT;
	rendererData.m_rastDesc     = PSOHandler::RASTERIZER_DEFAULT;
	rendererData.m_samplerDesc  = PSOHandler::SAMPLER_CLAMP;
	rendererData.m_inputLayout  = PSOHandler::IL_NONE;
	rendererData.m_depthFormat  = DXGI_FORMAT_UNKNOWN;
	rendererData.m_depthEnabled = false;
	rendererData.m_pixelShader  = L"Shaders/SSAOPS.cso";
	rendererData.m_format       = { DXGI_FORMAT_R8_UNORM };
	
	FullscreenTexture* ssaoTexture = m_textureRendering.CreateFullscreenTexture(
		L"SSAO Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8_UNORM,
		PRE_SCENE_PASS_0
	);
	TextureRenderer* ssaoRenderer  = m_textureRendering.CreateTextureRenderer(
		L"SSAO Renderer",
		rendererData,
		3,
		3,
		RENDER_PASS::PRE_SCENE_PASS_0
	);
	
	m_ssaoData.GenerateRandomTexture(textureHandler, m_bufferHandler, 64, 64);
	
	ssaoRenderer->SetRenderTargetAtSlot(ssaoTexture, 0);
	ssaoRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_WORLD_POSITION), 0, false);
	ssaoRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_NORMAL), 1, false);
	ssaoRenderer->SetTextureAtSlot(m_ssaoData.GetTextureOffset(), 2, false);
	ssaoRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(),   0);
	ssaoRenderer->SetBufferAtSlot(m_ssaoData.GetBufferOffset(), 1);
	ssaoRenderer->SetBufferAtSlot(m_ssaoData.GetCountOffset(),  2);

	rendererData.m_pixelShader = L"Shaders/VolumetricLightingPS.cso";
	rendererData.m_format      = { DXGI_FORMAT_R8G8B8A8_UNORM };
	
	m_vlData.InitializeConstantBuffer(m_bufferHandler);

	FullscreenTexture* volumetricTexture = m_textureRendering.CreateFullscreenTexture(
		L"Volumetric Light Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		PRE_SCENE_PASS_1
	);
	TextureRenderer* volumetricRenderer = m_textureRendering.CreateTextureRenderer(
		L"Volumetric Light Render Texture",
		rendererData, // All the Data compacted into a struct for more cleaner code
		3,			  // Number of Buffers
		2,			  // Number of Textures
		PRE_SCENE_PASS_1
	);

	// Set Render Target
	volumetricRenderer->SetRenderTargetAtSlot(volumetricTexture, 0);

	// Set Textures
	volumetricRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_WORLD_POSITION), 0, false);
	volumetricRenderer->SetTextureAtSlot(shadowMapTexture, 1);

	// Set Buffers
	volumetricRenderer->SetBufferAtSlot(m_cameraBuffer,		  0);
	volumetricRenderer->SetBufferAtSlot(m_lightBuffer,		  1);
	volumetricRenderer->SetBufferAtSlot(m_vlData.GetBuffer(), 2);

	rendererData.m_pixelShader = L"Shaders/SSAOBlurPS.cso";
	rendererData.m_format = { DXGI_FORMAT_R8_UNORM };

	FullscreenTexture* ssaoBlurTexture = m_textureRendering.CreateFullscreenTexture(
		L"SSAO Blur Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8_UNORM,
		PRE_SCENE_PASS_2
	);
	TextureRenderer* ssaoBlurRenderer = m_textureRendering.CreateTextureRenderer(
		L"SSAO Render Texture",
		rendererData,
		1,
		1,
		PRE_SCENE_PASS_2
	);
	
	ssaoBlurRenderer->SetRenderTargetAtSlot(ssaoBlurTexture, 0);
	ssaoBlurRenderer->SetTextureAtSlot(ssaoTexture, 0);
	ssaoBlurRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);

	rendererData.m_pixelShader = L"Shaders/DirectionalLightPS.cso";
	rendererData.m_format      = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	FullscreenTexture* dirLightTexture = m_textureRendering.CreateFullscreenTexture(
		L"Scene Light Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		SCENE_PASS_0
	);
	//m_textureRendering.m_lastRenderedTexture = dirLightTexture;
	FullscreenTexture* hdrBlurTexture = m_textureRendering.CreateFullscreenTexture(
		L"HDR Blur Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		SCENE_PASS_2
	);
	TextureRenderer* dirLightRenderer = m_textureRendering.CreateTextureRenderer(
		L"Directional Light Renderer",
		rendererData,
		2,
		11,
		SCENE_PASS_0
	);

	FullscreenTexture* pointLightTexture = m_textureRendering.CreateFullscreenTexture(
		L"Point Light Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		SCENE_PASS_0
	);
	FullscreenTexture* spotLightTexture = m_textureRendering.CreateFullscreenTexture(
		L"Spot Light Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		SCENE_PASS_0
	);
	dirLightRenderer->SetRenderTargetAtSlot(dirLightTexture, 0);
	dirLightRenderer->SetRenderTargetAtSlot(hdrBlurTexture,  1);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_ALBEDO),		   0, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_NORMAL),		   1, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_MATERIAL),	   2, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_VERTEX_COLOR),   3, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_VERTEX_NORMAL),  4, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_WORLD_POSITION), 5, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_DEPTH),		   6, false);
	dirLightRenderer->SetTextureAtSlot(textureHandler.GetTextureData(m_skyboxRenderer->TextureID()).m_offsetID, 7, false);
	dirLightRenderer->SetTextureAtSlot(shadowMapTexture,  8);
	dirLightRenderer->SetTextureAtSlot(ssaoBlurTexture,   9);
	dirLightRenderer->SetTextureAtSlot(volumetricTexture, 10);

	dirLightRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);
	dirLightRenderer->SetBufferAtSlot(m_lightBuffer->OffsetID(),  1);

	LightRenderer* lightRenderer = new LightRenderer(m_lightHandler);
	lightRenderer->InitializeRenderer(m_psoHandler, 2, 11);
	lightRenderer->SetRenderTargetAtSlot(pointLightTexture, 0);
	lightRenderer->SetRenderTargetAtSlot(spotLightTexture,  1);
	lightRenderer->SetRenderTargetAtSlot(hdrBlurTexture,    2);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_ALBEDO),			0, false);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_NORMAL),			1, false);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_MATERIAL),       2, false);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_VERTEX_COLOR),   3, false);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_VERTEX_NORMAL),  4, false);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_WORLD_POSITION), 5, false);
	lightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_DEPTH),			6, false);
	lightRenderer->SetTextureAtSlot(textureHandler.GetTextureData(m_skyboxRenderer->TextureID()).m_offsetID, 7, false);
	lightRenderer->SetTextureAtSlot(shadowMapTexture, 8);
	lightRenderer->SetTextureAtSlot(ssaoBlurTexture, 9);
	lightRenderer->SetTextureAtSlot(volumetricTexture, 10);

	lightRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);

	m_textureRendering.AddTextureRendererToPipeline(lightRenderer, SCENE_PASS_0);

	m_skyboxRenderer->SetRenderTargetAtSlot(dirLightTexture, 0);

	FullscreenTexture* bloomBlurTexture = m_textureRendering.CreateFullscreenTexture(
		L"Bloom Blur Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		POST_PROCESS_0
	);
	rendererData.m_pixelShader = L"Shaders/BloomBlurPS.cso";
	rendererData.m_format = { DXGI_FORMAT_R8G8B8A8_UNORM };
	TextureRenderer* bloomBlurRenderer = m_textureRendering.CreateTextureRenderer(
		L"Bloom Blur Renderer",
		rendererData,
		0,
		1,
		POST_PROCESS_0
	);
	bloomBlurRenderer->SetRenderTargetAtSlot(bloomBlurTexture, 0);
	bloomBlurRenderer->SetTextureAtSlot(hdrBlurTexture,  0);
	
	FullscreenTexture* bloomTexture = m_textureRendering.CreateFullscreenTexture(
		L"Bloom Texture",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		POST_PROCESS_1
	);
	rendererData.m_pixelShader = L"Shaders/BloomPS.cso";
	TextureRenderer* bloomRenderer = m_textureRendering.CreateTextureRenderer(
		L"Bloom  Renderer",
		rendererData,
		0,
		4,
		POST_PROCESS_1
	);
	bloomRenderer->SetRenderTargetAtSlot(bloomTexture, 0);
	bloomRenderer->SetTextureAtSlot(dirLightTexture,   0);
	bloomRenderer->SetTextureAtSlot(pointLightTexture, 1);
	bloomRenderer->SetTextureAtSlot(spotLightTexture,  2);
	bloomRenderer->SetTextureAtSlot(bloomBlurTexture,  3);

#if DEBUG
	m_debugRenderer = new DebugTextureRenderer(modelHandler);
	m_debugRenderer->InitializeDebugRenderer(
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		{ DXGI_FORMAT_R8G8B8A8_UNORM },
		m_psoHandler
	);
	m_debugRenderer->SetRenderingData(&m_debugRenderingData);
	m_debugRenderer->SetRenderTargetAtSlot(bloomTexture->RTVOffsetID(), 0);
	m_debugRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);
	m_textureRendering.AddTextureRendererToPipeline(m_debugRenderer, POST_PROCESS_1);
#endif
}

void RenderManager::Impl::RenderImgui()
{
#if EDITOR_DEBUG_VIEW // Draw last texture to ImGui window before rendering to backbuffer.
	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_framework.RTVHeap().GET_CPU_DESCRIPTOR(m_framework.m_frameIndex);
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
#else
	m_framework.RenderToBackBuffer(m_textureRendering.FetchLastRenderedTexture()->SRVOffsetID(), m_psoHandler);
#endif

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_framework.m_frameResources[m_framework.m_frameIndex]->CmdList());
}

void RenderManager::Impl::BeginFrame()
{
	ImGui::NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGuizmo::BeginFrame();

	m_framework.InitiateCommandList(nullptr, L"RenderManager Line " + std::to_wstring(__LINE__) + L"\n");
	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	// Transition resources for the correct usage
	{
		m_framework.QeueuResourceTransition(&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	
		ID3D12Resource* rtv[] = { m_framework.GetCurrentRTV() };
		m_framework.QeueuResourceTransition(&rtv[0], 1,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		m_textureRendering.TransitionResourceForRendering();
	}
		m_framework.TransitionAllResources();

	// Clear DepthStencil and ShadowDepthStencil
	{ 
		cmdList->RSSetViewports(1, &m_framework.m_viewport);
		cmdList->RSSetScissorRects(1, &m_framework.m_scissorRect);
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_framework.DSVHeap().GET_CPU_DESCRIPTOR(0);
		cmdList->ClearDepthStencilView(
			dsvHandle,
			D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
		);
	}
	
	// Clear all RenderTargets
	{
		cmdList->RSSetViewports(1, &m_framework.m_viewport);
		m_gBuffer.ClearRenderTargets(m_framework.RTVHeap(), cmdList, 0, nullptr);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_framework.RTVHeap().GET_CPU_DESCRIPTOR(0);
		rtvHandle.Offset(m_framework.m_frameIndex * m_framework.RTVHeap().DESCRIPTOR_SIZE());
		cmdList->ClearRenderTargetView(
			rtvHandle,
			&ClearColor[0], 
			1,
			&m_framework.m_scissorRect
		);
	}

	m_textureRendering.ClearAllTextures(cmdList);
}

void RenderManager::Impl::RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler,
	 SpriteHandler& spriteHandler, SceneManager& scene)
{	
	ImGui::Render();
	RenderScene(textureHandler, spriteHandler, modelHandler, effectHandler, scene);

	ClearAllInstances(modelHandler, spriteHandler);
}

PSOHandler& RenderManager::Impl::GetPSOHandler()
{
	return m_psoHandler;
}

TextureRenderingHandler& RenderManager::Impl::GetTextureRendering()
{
	return m_textureRendering;
}

BufferHandler& RenderManager::Impl::GetConstantBufferHandler()
{
	return m_bufferHandler;
}

void RenderManager::Impl::RenderScene(TextureHandler& textureHandler, SpriteHandler& spriteHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler, SceneManager& scene)
{
	modelHandler.LoadRequestedModels();

	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	const UINT frameIndex = m_framework.m_frameIndex;

	UpdatePrimaryConstantBuffers(scene);
    m_bufferHandler.UpdateBufferDataForRendering();

	Camera&             cam = Reg::Instance()->Get<Camera>(scene.GetMainCamera());
	Transform& camTransform = Reg::Instance()->Get<Transform>(scene.GetMainCamera());

	m_skyboxRenderer->FetchCameraPositionAndSkyboxModel(camTransform.m_position, modelHandler);
	ID3D12DescriptorHeap* descHeaps[] = {
		m_framework.CbvSrvHeap().GetDescriptorHeap(),
	};

	cmdList->SetDescriptorHeaps(_countof(descHeaps), &descHeaps[0]);
	
	{ //* Scene to Gbuffer start
		cam.ConstructFrustrum(camTransform.World(), camTransform.m_position); // Construct this frames Frustrum
		Update3DInstances(cam, scene, modelHandler, effectHandler);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_framework.DSVHeap().GET_CPU_DESCRIPTOR(0));
		std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> rtvHandle = m_gBuffer.GetRTVDescriptorHandles(m_framework.RTVHeap());
		cmdList->OMSetRenderTargets(GBUFFER_COUNT, &rtvHandle[0], false, &dsvHandle);
				
		cmdList->SetGraphicsRootSignature(m_psoHandler.GetRootSignature(m_gBuffer.GetRootSignature()));
		cmdList->SetPipelineState(m_psoHandler.GetPipelineState(m_gBuffer.GetPSO()));

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
		// Render non Transparent effects

		{ // Copy over Depth buffer data to a depth texture to be used in pixel shader
			ID3D12Resource* depthCopy[]     = { m_framework.m_depthStencil.Get() };
			ID3D12Resource* depthDest[]     = { effectHandler.GetDepthCopyTexture() };
			m_framework.QeueuResourceTransition(&depthCopy[0], 1, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COPY_SOURCE);
			m_framework.QeueuResourceTransition(&depthDest[0], 1, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
			m_framework.TransitionAllResources();

			cmdList->CopyResource(effectHandler.GetDepthCopyTexture(), m_framework.m_depthStencil.Get());

			m_framework.QeueuResourceTransition(&depthCopy[0], 1, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			m_framework.QeueuResourceTransition(&depthDest[0], 1, D3D12_RESOURCE_STATE_COPY_DEST,   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			m_framework.TransitionAllResources();
		}

		m_mainRenderer.RenderForwardModelEffects(
			cmdList,
			m_psoHandler,
			effectHandler.GetDepthTextureOffset(),
			effectHandler.GetAllEffects(), 
			modelHandler, textureHandler.GetTextures(), 
			frameIndex, 
			cam,
			camTransform,
			0
		);

		m_shadowMap->SetModelsData(modelHandler.GetAllModels());

		m_framework.QeueuResourceTransition(
			&m_gBuffer.GetGbufferResources()[0], GBUFFER_COUNT,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	} //* Scene to Gbuffer end

	m_textureRendering.RenderPass(PRE_SCENE_PASS_0, PRE_SCENE_PASS_2, cmdList);
	m_textureRendering.RenderPass(SCENE_PASS_0, SCENE_PASS_2, cmdList);
	m_textureRendering.RenderPass(POST_PROCESS_0, POST_PROCESS_2, cmdList);


	//* Render 2D Scene
	{ 
		//cmdList->SetGraphicsRootSignature(m_psoHandler.GetRootSignature(m_2dRenderer.GetRootSignature()));
		//cmdList->SetPipelineState(m_psoHandler.GetPipelineState(m_2dRenderer.GetPSO()));
		//
		//Update2DInstances(scene, spriteHandler);
		//m_2dRenderer.RenderUI(
		//	cmdList, 
		//	spriteHandler.GetAllSprites(), 
		//	m_framework.GetFrameIndex(), 
		//	textureHandler.GetTextures()
		//);
	} 
	//* Render 2D Scene End

	RenderImgui(); // Render Imgui over everything else
	

	m_framework.TransitionRenderTarget(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void RenderManager::Impl::FrustrumCulling(const Camera& camera, Transform& transform, ModelData& model)
{
	for (uint32_t i = 0; i < model.GetMeshes().size(); i++) {
		
		const FrustrumCulling::AABB& aabb = model.GetSingleMesh(i).m_aabb;
		//if (FrustrumCulling::MeshIsInFrustrum(camera, transform, aabb, m_debugRenderingData)) {
	    model.AddMeshToRender(i, transform.World());
		//}
	}
}

void RenderManager::Impl::Update3DInstances(const Camera& cam, SceneManager& scene, ModelHandler& modelHandler, ModelEffectHandler& effectHandler)
{	
	auto view = Reg::Instance()->GetRegistry().view<Transform, GameEntity>();

	for (auto entity : view)
	{
		GameEntity& obj = Reg::Instance()->Get<GameEntity>(entity);

		if (obj.m_state == GameEntity::STATE::ACTIVE) {
			Transform& transform = view.get<Transform>(entity);
			const Vector3& pos   = transform.m_position;
			const Vector3& scale = transform.m_scale;

			Mat4f mat;
			DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&transform.m_rotation);
			mat *= DirectX::XMMatrixScaling(transform.m_scale.x, transform.m_scale.y, transform.m_scale.z);
			mat *= DirectX::XMMatrixRotationQuaternion(quatv);
			mat *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

			transform.m_local = mat;
			transform.m_world = !transform.Parent() ? transform.m_world = transform.m_local : transform.m_local * transform.Parent()->m_world;

			Model* model = Reg::Instance()->TryGet<Model>(entity);
			if (model) {
				if (model->m_modelID != UINT32_MAX) {
					//Transform& transform = reg.get<Transform>(entity);
						
					if (!Reg::Instance()->TryGet<ModelEffect>(entity)) {

						ModelData& modelData = modelHandler.GetLoadedModelInformation(model->m_modelID);
						
						if (modelData.IsTransparent()) {
						}
						else {
							FrustrumCulling(cam, transform, modelData);
						}
					}
					else {
						ModelEffect&     effect     = Reg::Instance()->Get<ModelEffect>(entity);
						ModelEffectData& effectData = effectHandler.GetModelEffectData(effect.m_effectID);
						effectData.GetTransforms().emplace_back(DirectX::XMMatrixTranspose(transform.m_world));
					}
				}
			}
		}	
	}

	auto pointLightView = REGISTRY->GetRegistry().view<Transform, GameEntity, PointLight>();
	for (auto entity : pointLightView)
	{
		const Transform&  transform  = REGISTRY->Get<Transform>(entity);
		const PointLight& pointLight = REGISTRY->Get<PointLight>(entity);

		m_lightHandler.AddLightInstanceForRendering(pointLight, transform);
	}
	
	auto spotLightView = REGISTRY->GetRegistry().view<Transform, GameEntity, SpotLight>();
	for (auto entity : spotLightView)
	{
		Transform& transform       = REGISTRY->Get<Transform>(entity);
		const SpotLight& spotLight = REGISTRY->Get<SpotLight>(entity);

		m_lightHandler.AddLightInstanceForRendering(spotLight, transform);
	}

	// Show debug lines for camera...
	// Near
	//Vect3f ltn = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::LEFT_FACE),
	//	cam.GetFrustrumFace(Camera::TOP_FACE),
	//	cam.GetFrustrumFace(Camera::NEAR_FACE)
	//);
	//
	//Vect3f rtn = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::RIGHT_FACE),
	//	cam.GetFrustrumFace(Camera::TOP_FACE),
	//	cam.GetFrustrumFace(Camera::NEAR_FACE)
	//);
	//
	//Vect3f lbn = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::LEFT_FACE),
	//	cam.GetFrustrumFace(Camera::BOTTOM_FACE),
	//	cam.GetFrustrumFace(Camera::NEAR_FACE)
	//);
	//
	//Vect3f rbn = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::RIGHT_FACE),
	//	cam.GetFrustrumFace(Camera::BOTTOM_FACE),
	//	cam.GetFrustrumFace(Camera::NEAR_FACE)
	//);
	//
	//// Far
	//Vect3f ltf = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::LEFT_FACE),
	//	cam.GetFrustrumFace(Camera::TOP_FACE),
	//	cam.GetFrustrumFace(Camera::FAR_FACE)
	//);
	//
	//Vect3f rtf = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::RIGHT_FACE),
	//	cam.GetFrustrumFace(Camera::TOP_FACE),
	//	cam.GetFrustrumFace(Camera::FAR_FACE)
	//);
	//
	//Vect3f lbf = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::LEFT_FACE),
	//	cam.GetFrustrumFace(Camera::BOTTOM_FACE),
	//	cam.GetFrustrumFace(Camera::FAR_FACE)
	//);
	//
	//Vect3f rbf = PlaneIntersecting(
	//	cam.GetFrustrumFace(Camera::RIGHT_FACE),
	//	cam.GetFrustrumFace(Camera::BOTTOM_FACE),
	//	cam.GetFrustrumFace(Camera::FAR_FACE)
	//);
	//
	//// Near 
	//{
	//	m_debugRenderingData.RenderLine(rtn, ltn);
	//	m_debugRenderingData.RenderLine(rbn, lbn);
	//	m_debugRenderingData.RenderLine(rtn, rbn);
	//	m_debugRenderingData.RenderLine(ltn, lbn);
	//}
	//
	//// Sides
	//{
	//	m_debugRenderingData.RenderLine(ltf, ltn);
	//	m_debugRenderingData.RenderLine(lbf, lbn);
	//	m_debugRenderingData.RenderLine(rtf, rtn);
	//	m_debugRenderingData.RenderLine(rbf, rbn);
	//}
	//
	//// Far
	//{
	//	m_debugRenderingData.RenderLine(rtf, ltf);
	//	m_debugRenderingData.RenderLine(rbf, lbf);
	//	m_debugRenderingData.RenderLine(rtf, rbf);
	//	m_debugRenderingData.RenderLine(ltf, lbf);
	//}
	
	//// Normals of planes
	//{
	//	Vect3f normal = cam.GetFrustrumFace(Camera::RIGHT_FACE).Normal();
	//	m_debugRenderingData.RenderLine(Vect3f::Zero, normal * 10.0f, { 0.0f, 0.0f, 1.0f, 1.0f });
	//
	//	normal = cam.GetFrustrumFace(Camera::LEFT_FACE).Normal();
	//	m_debugRenderingData.RenderLine(Vect3f::Zero , normal * 10.0f, { 0.0f, 1.0f, 1.0f, 1.0f });
	//	
	//	normal = cam.GetFrustrumFace(Camera::BOTTOM_FACE).Normal();
	//	m_debugRenderingData.RenderLine(Vect3f::Zero, normal * 10.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	//
	//	normal = cam.GetFrustrumFace(Camera::NEAR_FACE).Normal();
	//	m_debugRenderingData.RenderLine(Vect3f::Zero, normal * 10.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
	//
	//	//normal = cam.GetFrustrumFace(Camera::FAR_FACE).Normal();
	//	//m_debugRenderingData.RenderLine(Vect3f::Zero, normal * 10.0f, { 0.8f, 0.2f, 1.0f, 1.0f });
	//}

}
void RenderManager::Impl::Update2DInstances(SceneManager& scene, SpriteHandler& spriteHandler)
{
	//entt::registry& reg = scene.Registry();
	//
	//auto spriteView = reg.view<Transform2D, Sprite>();
	//for (auto entity : spriteView) {
	//	GameEntity& obj = reg.get<GameEntity>(entity);
	//	if(obj.m_state == GameEntity::STATE::ACTIVE){
	//		Sprite& sprite = reg.get<Sprite>(entity);
	//		AddSpriteSheetInstance(sprite, reg.get<Transform2D>(entity), spriteHandler);
	//	}
	//}
	//
	//auto textView = reg.view<Transform2D, GameEntity, TextOLD>();
	//for (auto entity : textView) {
	//	GameEntity& obj = reg.get<GameEntity>(entity);
	//	if (obj.m_state == GameEntity::STATE::ACTIVE) {
	//		TextOLD& text = reg.get<TextOLD>(entity);
	//		if (text.m_fontID != 0) {
	//			AddFontInstance(text, reg.get<Transform2D>(entity), spriteHandler);
	//		}
	//	}
	//}
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

void RenderManager::Impl::AddFontInstance(TextOLD& data, Transform2D& transform, SpriteHandler& spriteHandler)
{
	Vect2f newPosition = {0.f, 0.f};	
	std::string& text = data.m_text;

	std::vector<Font>& fonts = spriteHandler.GetAllFonts();

	for (UINT j = 0; j < text.size(); j++)
	{
		char chr = text[j];
		Font::Letter letterData = fonts[data.m_fontID - 1].GetData().m_letters[chr];

		Font::Instance inst;

		inst.m_fontPosition = { letterData.m_position.x, letterData.m_position.y, 0.f, 1.f};
		inst.m_fontSize     = { (float)letterData.m_width, (float)letterData.m_height };
		inst.m_sheetSize    = { (float)fonts[data.m_fontID - 1].GetData().m_width, (float)fonts[data.m_fontID - 1].GetData().m_height };
		inst.m_position     = newPosition;
		inst.m_color		= data.m_color;
		inst.m_size			= transform.m_scale;
		inst.m_anchor		= transform.m_position;
	
		newPosition.x += 1.1f;
		fonts[data.m_fontID - 1].AddInstance(inst);
	}
}

void RenderManager::Impl::UpdatePrimaryConstantBuffers(SceneManager& scene)
{
	Camera&    cam          = Reg::Instance()->Get<Camera>(scene.GetMainCamera());
	Transform& camTransform = Reg::Instance()->Get<Transform>(scene.GetMainCamera());

	auto list = Reg::Instance()->GetRegistry().view<DirectionalLight, Transform, GameEntity>();
	DirectionalLight directionalLight;
	Vect4f dirLightPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	Vect4f directionaLightdir = { 0.f, 0.f, 0.f, 1.f };

	LightDefaultBuffer lightData;
	DefaultBuffer      camData;
	DefaultBuffer      shadowData;

	camData.m_transformTwo = DirectX::XMMatrixTranspose(camTransform.m_world.Invert());
	camData.m_transformOne = DirectX::XMMatrixTranspose(cam.GetProjection());
	camData.m_vectorOne	   = camTransform.m_position;
	camData.m_vectorTwo	   = camData.m_transformTwo.Forward();
	camData.m_vectorOne.w  = static_cast<float>(cam.RenderTarget());
	camData.m_time		   = clock() / 1000.0f;
	camData.m_windowSize   = {
		static_cast<float>(WindowHandler::WindowData().m_w),
		static_cast<float>(WindowHandler::WindowData().m_h)
	};

	for (auto entity : list) {
		directionalLight				  = Reg::Instance()->Get<DirectionalLight>(entity);
		directionalLight.m_lightTransform = Reg::Instance()->Get<Transform>(entity).m_local;
		dirLightPos						  = Reg::Instance()->Get<Transform>(entity).m_position;

		lightData.m_lightTransform = DirectX::XMMatrixTranspose(directionalLight.m_lightTransform.Invert());
		lightData.m_lightColor     = directionalLight.m_lightColor;
		lightData.m_ambientColor   = directionalLight.m_ambientColor;
		lightData.m_direction      = lightData.m_lightTransform.Forward();
	}
	lightData.m_lightProjection = DirectX::XMMatrixTranspose(m_shadowMap->GetProjectionMatrix());

	shadowData.m_transformTwo = lightData.m_lightTransform;
	shadowData.m_transformOne = lightData.m_lightProjection;
	shadowData.m_vectorOne = dirLightPos;

	m_cameraBuffer->UpdateBufferData(reinterpret_cast<uint16_t*>(&camData));
	m_lightBuffer->UpdateBufferData(reinterpret_cast<uint16_t*>(&lightData));
	m_shadowMapBuffer->UpdateBufferData(reinterpret_cast<uint16_t*>(&shadowData));
}

void RenderManager::Impl::ClearAllInstances(ModelHandler& modelHandler, SpriteHandler& spriteHandler)
{
	for (auto& model : modelHandler.GetAllModels())
		model.ClearInstanceTransform();
	
	for (auto& sprite : spriteHandler.GetAllSprites())
		sprite.ClearInstances();
	
	for (auto& font : spriteHandler.GetAllFonts())
		font.ClearInstances();
}

RenderManager::RenderManager(D3D12Framework& framework, TextureHandler& textureHandler)
{
	m_Impl = new Impl(framework, textureHandler);
}

RenderManager::~RenderManager()
{
	m_Impl->~Impl();
}

void RenderManager::InitializeRenderTextures(TextureHandler& textureHandler, ModelHandler& modelHandler)
{
	m_Impl->InitializeRenderTextures(textureHandler, modelHandler);
}

void RenderManager::BeginFrame()
{
	m_Impl->BeginFrame();
}

void RenderManager::RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler, SpriteHandler& SpriteHandler, SceneManager& scene)
{
	m_Impl->RenderFrame(textureHandler, modelHandler, effectHandler, SpriteHandler, scene);
}

PSOHandler& RenderManager::GetPSOHandler() const noexcept
{
	return m_Impl->GetPSOHandler();
}

BufferHandler& RenderManager::GetConstantHandler() const noexcept
{
	return m_Impl->GetConstantBufferHandler();
}

TextureRenderingHandler& RenderManager::GetTextureRendering() const noexcept
{
	return m_Impl->GetTextureRendering();
}

void RenderManager::SetKuwaharaRadius(UINT radius, UINT scale, Vect3f offset) {
	m_Impl->SetKuwaharaRadius(radius, scale, offset);
}