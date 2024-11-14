#include "stdafx.h"

#include "Renderer.h"
#include "Rendering/2D/2DRenderer.h"

#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"

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
#include "Screen Rendering/LightHandler.h"
#include "Screen Rendering/DirectionalShadowMapping.h"
#include "Screen Rendering/Fullscreen Shaders/KuwaharaFilter.h"
#include "Screen Rendering/Textures/TextureRenderingHandler.h"

#include "VolumetricLightData.h"
#include "SSAOData.h"
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
		LightHandler& lightHandler, TextureHandler& textureHandler, ModelHandler& modelHandler, 
		ModelEffectHandler& effectHandler, SpriteHandler& SpriteHandler, SceneManager& scene/*Camera, Ligthing, GameObjects, etc...*/
	);

	PSOHandler&				 GetPSOHandler();
	TextureRenderingHandler& GetTextureRendering();
	BufferHandler&			 GetConstantBufferHandler();

	void SetKuwaharaRadius(UINT radius, UINT scale, Vect3f offset) {
		m_kuwaharaFilter.SetFilterRadius(radius, scale, offset);
	}

private:
	void RenderScene(LightHandler& lightHandler, TextureHandler& textureHandler, SpriteHandler& spriteHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler, SceneManager& scene);

	void Render3DInstances(SceneManager& scene, ModelHandler& modelHandler, TextureHandler& textureHandler, ModelEffectHandler& effectHandler);

	void Update3DInstances(SceneManager& scene, ModelHandler& modelHandler, ModelEffectHandler& effectHandler);
	void Update2DInstances(SceneManager& scene, SpriteHandler& spriteHandler);

	void AddSpriteSheetInstance(Sprite& data, Transform2D& transform, SpriteHandler& spriteHandler);
	void AddFontInstance(Text& data, Transform2D& trransform, SpriteHandler& spriteHandler);

	void UpdatePrimaryConstantBuffers(SceneManager& scene);
	void ClearAllInstances(ModelHandler& modelHandler, SpriteHandler& spriteHandler);

	PSOHandler	m_psoHandler;
	Renderer	m_mainRenderer;
	Renderer2D	m_2dRenderer;
	GBuffer		m_gBuffer;
	
	UINT m_lightRootSignture;
	UINT m_dirLightPSO;
	UINT m_pointLightPSO;

	D3D12Framework&          m_framework;
	TextureRenderingHandler  m_textureRendering;
	BufferHandler			 m_bufferHandler;

	SSAOData				 m_ssaoData;
	VolumetricLightData      m_vlData;

	TextureRenderer*         m_dirLightTexture = nullptr;

	ConstantBufferData*      m_cameraBuffer			   = nullptr;
	ConstantBufferData*		 m_shadowMapBuffer		   = nullptr;
	ConstantBufferData*		 m_fullscreenTextureBuffer = nullptr;
	ConstantBufferData*		 m_lightBuffer			   = nullptr;

	Skybox*					 m_skyboxRenderer = nullptr;
	KuwaharaFilter			 m_kuwaharaFilter;
	DirectionalShadowMapping* m_shadowMap = nullptr;

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
	m_shadowMap()
{
	// Set the back buffer PSO and RS for use at the end of a frame.
	m_framework.SetBackBufferPSO(m_psoHandler);

	// Setup the main rendering components
	m_2dRenderer.Init(framework, m_psoHandler);
	m_mainRenderer.Init(framework);

	//m_kuwaharaFilter.InitializeFullscreenShader(framework.GetDevice(), &framework.CbvSrvHeap());
	//m_kuwaharaFilter.CreatePipelineAndRootsignature(m_psoHandler);
	//m_kuwaharaFilter.SetFilterRadius(1, 1, {0.0f, 0.0f, 0.0f});
	// Setup of 
	//m_dirLight.InitAsTexture(
	//	framework.GetDevice(),
	//	&framework.CbvSrvHeap(),
	//	&framework.RTVHeap(),
	//	WindowHandler::WindowData().m_w,
	//	WindowHandler::WindowData().m_h,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
	//	L"Directional Light Buffer"
	//);
	//m_dirLight.SetPipelineAndRootSignature(m_psoHandler);
	//m_dirLight.InitBuffers(framework.GetDevice(), framework.CbvSrvHeap());
	//m_ssao.InitAsTexture(
	//	framework.GetDevice(),
	//	&framework.CbvSrvHeap(),
	//	&framework.RTVHeap(),
	//	WindowHandler::WindowData().m_w, 
	//	WindowHandler::WindowData().m_h, 
	//	DXGI_FORMAT_R32_FLOAT,
	//	DXGI_FORMAT_R32_FLOAT,
	//	D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
	//	L"SSAO Texture Buffer"
	//);
	//m_ssao.SetPipelineAndRootSignature(m_psoHandler);
	//m_ssao.InitializeSSAO(framework, textureHandler, 64, 64);
	//
	//m_ssaoBlur.InitAsTexture(
	//	framework.GetDevice(),
	//	&framework.CbvSrvHeap(),
	//	&framework.RTVHeap(),
	//	WindowHandler::WindowData().m_w,
	//	WindowHandler::WindowData().m_h,
	//	DXGI_FORMAT_R32_FLOAT,
	//	DXGI_FORMAT_R32_FLOAT,
	//	D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
	//	L"SSAO Blur Texture Buffer"
	//);
	//m_ssaoBlur.SetPipelineAndRootSignature(m_psoHandler);
	//
	//m_volumetricLight.InitAsTexture(
	//	framework.GetDevice(),
	//	&framework.CbvSrvHeap(),
	//	&framework.RTVHeap(),
	//	WindowHandler::WindowData().m_w,
	//	WindowHandler::WindowData().m_h,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
	//	L"Volumetric Light Texture Buffer"
	//);
	//m_volumetricLight.SetPipelineAndRootSignature(m_psoHandler);
	//m_volumetricLight.InitBuffers(framework.GetDevice(), framework.CbvSrvHeap());
	//m_volumetricLight.SetVolumetricData(100, 0.35f, 1.5f);
	//
	//m_volumetricLightBlur.InitAsTexture(
	//	framework.GetDevice(),
	//	&framework.CbvSrvHeap(),
	//	&framework.RTVHeap(),
	//	WindowHandler::WindowData().m_w,
	//	WindowHandler::WindowData().m_h,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
	//	L"Volumetric Light Blur Texture Buffer"
	//);
	//m_volumetricLightBlur.SetPipelineAndRootSignature(m_psoHandler);
	//m_volumetricLightBlur.InitBuffers(framework.GetDevice(), framework.CbvSrvHeap());
	//
	//DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	//CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	//depth.DepthEnable = false;
	//auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
	//		     D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
	//		     D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
	//		     D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	//m_lightRootSignture = m_psoHandler.CreateRootSignature(2, GBUFFER_COUNT + 4, PSOHandler::SAMPLER_CLAMP, flags, L"Light Root Signature");
	//m_dirLightPSO = m_psoHandler.CreatePSO(
	//	{L"Shaders/FullscreenVS.cso", L"Shaders/DirectionalLightPS.cso"},
	//	PSOHandler::BLEND_DEFAULT,
	//	PSOHandler::RASTERIZER_DEFAULT,
	//	depth,
	//	DXGI_FORMAT_UNKNOWN,
	//	&format[0],
	//	1,
	//	m_lightRootSignture,
	//	PSOHandler::IL_NONE,
	//	L"Directional Light PSO"
	//);
	//m_pointLightPSO = m_psoHandler.CreatePSO(
	//	{ L"Shaders/FullscreenVS.cso", L"Shaders/PointLightPS.cso" },
	//	PSOHandler::BLEND_DEFAULT,
	//	PSOHandler::RASTERIZER_DEFAULT,
	//	depth,
	//	DXGI_FORMAT_UNKNOWN,
	//	&format[0],
	//	1,
	//	m_lightRootSignture,
	//	PSOHandler::IL_NONE,
	//	L"Point Light PSO"
	//);

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

	m_skyboxRenderer = new Skybox(modelHandler);
	m_skyboxRenderer->Initialize(m_psoHandler, textureHandler);
	m_skyboxRenderer->SetBufferAtSlot(m_cameraBuffer, 0);
	m_textureRendering.AddTextureRendererToPipeline(m_skyboxRenderer, SCENE_PASS_0);

	m_shadowMap = new DirectionalShadowMapping();
	m_shadowMap->SetPipelineAndRootSignature(m_psoHandler);
	m_shadowMap->SetBufferAtSlot(m_shadowMapBuffer->OffsetID(), 0, true);
	m_shadowMap->CreateProjection(64.0f, 4.0f);
	m_textureRendering.AddTextureRendererToPipeline(m_shadowMap, PRE_SCENE_PASS_0);
	FullscreenTexture* shadowMapTexture = m_textureRendering.CreateFullscreenTexture(
		8192,
		8192,
		DXGI_FORMAT_R32_TYPELESS,
		DXGI_FORMAT_R32_FLOAT,
		L"Shadow Map Texture",
		PRE_SCENE_PASS_0,
		true
	);
	m_shadowMap->SetDepthStencilView(shadowMapTexture->DSVOffsetID());

	TexturePipelineData textureData;
	textureData.m_blendDesc    = PSOHandler::BLEND_DEFAULT;
	textureData.m_rastDesc     = PSOHandler::RASTERIZER_DEFAULT;
	textureData.m_samplerDesc  = PSOHandler::SAMPLER_CLAMP;
	textureData.m_inputLayout  = PSOHandler::IL_NONE;
	textureData.m_depthFormat  = DXGI_FORMAT_UNKNOWN;
	textureData.m_depthEnabled = false;

	textureData.m_pixelShader  = L"Shaders/SSAOPS.cso";
	textureData.m_format       = { DXGI_FORMAT_R8_UNORM };
	
	FullscreenTexture* ssaoTexture = m_textureRendering.CreateFullscreenTexture(
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
		L"SSAO Texture",
		PRE_SCENE_PASS_0
	);
	TextureRenderer* ssaoRenderer  = m_textureRendering.CreateTextureRenderer(
		textureData,
		3,
		3,
		L"SSAO Renderer",
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

	textureData.m_pixelShader = L"Shaders/VolumetricLightingPS.cso";
	textureData.m_format      = { DXGI_FORMAT_R8G8B8A8_UNORM };

	FullscreenTexture* volumetricTexture = m_textureRendering.CreateFullscreenTexture(
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		L"Volumetric Light Texture",
		PRE_SCENE_PASS_1
	);
	TextureRenderer* volumetricRenderer = m_textureRendering.CreateTextureRenderer(
		textureData,
		3,
		2,
		L"Volumetric Light Render Texture",
		PRE_SCENE_PASS_1
	);

	m_vlData.InitializeConstantBuffer(m_bufferHandler);
	volumetricRenderer->SetRenderTargetAtSlot(volumetricTexture, 0);
	volumetricRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_WORLD_POSITION), 0, false);
	volumetricRenderer->SetTextureAtSlot(shadowMapTexture, 1);
	volumetricRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);
	volumetricRenderer->SetBufferAtSlot(m_lightBuffer->OffsetID(),  1);
	volumetricRenderer->SetBufferAtSlot(m_vlData.GetBuffer(),       2);

	textureData.m_pixelShader = L"Shaders/SSAOBlurPS.cso";
	textureData.m_format = { DXGI_FORMAT_R8_UNORM };

	FullscreenTexture* ssaoBlurTexture = m_textureRendering.CreateFullscreenTexture(
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
		L"SSAO Blur Texture",
		PRE_SCENE_PASS_1
	);
	TextureRenderer* ssaoBlurRenderer = m_textureRendering.CreateTextureRenderer(
		textureData,
		1,
		1,
		L"SSAO Render Texture",
		PRE_SCENE_PASS_2
	);
	
	ssaoBlurRenderer->SetRenderTargetAtSlot(ssaoBlurTexture, 0);
	ssaoBlurRenderer->SetTextureAtSlot(ssaoTexture, 0);
	ssaoBlurRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);

	textureData.m_pixelShader = L"Shaders/DirectionalLightPS.cso";
	textureData.m_format      = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	FullscreenTexture* dirLightTexture = m_textureRendering.CreateFullscreenTexture(
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		L"Scene Light Texture",
		SCENE_PASS_2
	);
	m_textureRendering.m_lastRenderedTexture = dirLightTexture;
	FullscreenTexture* hdrBlurTexture = m_textureRendering.CreateFullscreenTexture(
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		L"HDR Blur Texture",
		SCENE_PASS_2
	);
	TextureRenderer* dirLightRenderer = m_textureRendering.CreateTextureRenderer(
		textureData,
		2,
		11,
		L"Directional Light Render Texture",
		SCENE_PASS_0
	);

	dirLightRenderer->SetRenderTargetAtSlot(dirLightTexture, 0);
	dirLightRenderer->SetRenderTargetAtSlot(hdrBlurTexture,  1);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_ALBEDO), 0, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_NORMAL), 1, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_MATERIAL), 2, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_VERTEX_COLOR), 3, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_VERTEX_NORMAL), 4, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_WORLD_POSITION), 5, false);
	dirLightRenderer->SetTextureAtSlot(m_gBuffer.GetSRVOffset(GBUFFER_DEPTH), 6, false);
	dirLightRenderer->SetTextureAtSlot(textureHandler.GetTextureData(m_skyboxRenderer->TextureID()).m_offsetID, 7, false);
	dirLightRenderer->SetTextureAtSlot(shadowMapTexture,  8);
	dirLightRenderer->SetTextureAtSlot(ssaoBlurTexture,   9);
	dirLightRenderer->SetTextureAtSlot(volumetricTexture, 10);

	dirLightRenderer->SetBufferAtSlot(m_cameraBuffer->OffsetID(), 0);
	dirLightRenderer->SetBufferAtSlot(m_lightBuffer->OffsetID(),  1);

	m_skyboxRenderer->SetRenderTargetAtSlot(dirLightTexture, 0);
}

void RenderManager::Impl::RenderImgui()
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

	m_framework.InitiateCommandList(nullptr, L"RenderManager Line " + std::to_wstring(__LINE__) + L"\n");
	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	m_textureRendering.SetCurrentFrameindex(m_framework.m_frameIndex);

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

// DirectX12Framework pipeline needs to be fully reworked it seems >:(

void RenderManager::Impl::RenderFrame(LightHandler& lightHandler, TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler,
	 SpriteHandler& spriteHandler, SceneManager& scene)
{	
	ImGui::Render();
	RenderScene(lightHandler, textureHandler, spriteHandler, modelHandler, effectHandler, scene);

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

void RenderManager::Impl::RenderScene(LightHandler& lightHandler, TextureHandler& textureHandler, SpriteHandler& spriteHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler, SceneManager& scene)
{
	ID3D12GraphicsCommandList* cmdList = m_framework.CurrentFrameResource()->CmdList();

	const UINT frameIndex = m_framework.m_frameIndex;

	entt::registry& reg = scene.Registry();
	UpdatePrimaryConstantBuffers(scene);
    m_bufferHandler.UpdateBufferDataForRendering();

	Camera& cam = reg.get<Camera>(scene.GetMainCamera());
	Transform& camTransform = reg.get<Transform>(scene.GetMainCamera());

	m_skyboxRenderer->FetchCameraPositionAndSkyboxModel(camTransform.m_position, modelHandler);
	ID3D12DescriptorHeap* descHeaps[] = {
		m_framework.CbvSrvHeap().GetDescriptorHeap(),
	};

	cmdList->SetDescriptorHeaps(_countof(descHeaps), &descHeaps[0]);
	
	{ //* Scene to Gbuffer start
		Update3DInstances(scene, modelHandler, effectHandler);

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

	m_framework.RenderToBackBuffer(m_textureRendering.FetchLastRenderedTexture()->SRVOffsetID(), m_psoHandler);

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

void RenderManager::Impl::Update3DInstances(SceneManager& scene, ModelHandler& modelHandler, ModelEffectHandler& effectHandler)
{
	entt::registry& reg = scene.Registry();
	Transform& cam = reg.get<Transform>(scene.GetMainCamera());

	auto view = reg.view<Transform, Model, GameEntity>();

	for (auto entity : view)
	{
		GameEntity& obj = reg.get<GameEntity>(entity);

		if (obj.m_state == GameEntity::STATE::ACTIVE) {
			Model& model = reg.get<Model>(entity);
			if (model.m_modelID != 0) {
				Transform& transform = reg.get<Transform>(entity);
				if (!reg.try_get<ModelEffect>(entity)) {
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
				else {
					ModelEffect& effect = reg.get<ModelEffect>(entity);
					ModelEffectData& effectData = effectHandler.GetModelEffectData(effect.m_effectID);
					effectData.GetTransforms().emplace_back(DirectX::XMMatrixTranspose(transform.m_world));
				}
			}
		}	
	}	

}
void RenderManager::Impl::Update2DInstances(SceneManager& scene, SpriteHandler& spriteHandler)
{
	entt::registry& reg = scene.Registry();

	auto spriteView = reg.view<Transform2D, Sprite>();
	for (auto entity : spriteView) {
		GameEntity& obj = reg.get<GameEntity>(entity);
		if(obj.m_state == GameEntity::STATE::ACTIVE){
			Sprite& sprite = reg.get<Sprite>(entity);
			AddSpriteSheetInstance(sprite, reg.get<Transform2D>(entity), spriteHandler);
		}
	}

	auto textView = reg.view<Transform2D, GameEntity, Text>();
	for (auto entity : textView) {
		GameEntity& obj = reg.get<GameEntity>(entity);
		if (obj.m_state == GameEntity::STATE::ACTIVE) {
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

void RenderManager::Impl::UpdatePrimaryConstantBuffers(SceneManager& scene)
{
	entt::registry& reg = scene.Registry();
	Camera& cam = reg.get<Camera>(scene.GetMainCamera());
	Transform& camTransform = reg.get<Transform>(scene.GetMainCamera());

	auto list = scene.Registry().view<DirectionalLight, Transform>();
	DirectionalLight directionalLight;
	Vect4f dirLightPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	Vect4f directionaLightdir = { 0.f, 0.f, 0.f, 1.f };

	LightDefaultBuffer lightData;
	DefaultBuffer      camData;
	DefaultBuffer      shadowData;

	camData.m_transformTwo = DirectX::XMMatrixTranspose(camTransform.m_world.Invert());
	camData.m_transformOne = DirectX::XMMatrixTranspose(cam.GetProjection());
	camData.m_vectorOne = camTransform.m_position;
	camData.m_vectorTwo = -camData.m_transformTwo.Forward();
	camData.m_vectorOne.w = cam.RenderTarget();
	camData.m_time = clock() / 1000.0f;
	camData.m_windowSize = {
		static_cast<float>(WindowHandler::WindowData().m_w),
		static_cast<float>(WindowHandler::WindowData().m_h)
	};

	for (auto entity : list) {
		directionalLight = reg.get<DirectionalLight>(entity);
		directionalLight.m_lightTransform = reg.get<Transform>(entity).m_world;
		dirLightPos = reg.get<Transform>(entity).m_position;

		lightData.m_lightTransform = DirectX::XMMatrixTranspose(directionalLight.m_lightTransform.Invert());
		lightData.m_lightColor = directionalLight.m_lightColor;
		lightData.m_ambientColor = directionalLight.m_ambientColor;
		lightData.m_direction = lightData.m_lightTransform.Forward();
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

void RenderManager::RenderFrame(LightHandler& lightHandler, TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler, SpriteHandler& SpriteHandler, SceneManager& scene)
{
	m_Impl->RenderFrame(lightHandler, textureHandler, modelHandler, effectHandler, SpriteHandler, scene);
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

