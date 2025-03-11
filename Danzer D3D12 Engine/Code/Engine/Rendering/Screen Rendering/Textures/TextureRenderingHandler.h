#pragma once

#include "FullscreenTexture.h"

struct ID3D12GraphicsCommandList;

class TextureRenderer;
class DescriptorHeapWrapper;
class PSOHandler; 

// All the passes in the rendering pipeline, the order of first to last depending on each pass is 0 - 2 (start to end)
enum RENDER_PASS : uint8_t {
	PRE_SCENE_PASS_0, // 1st pass in PRE_SCENE_PASS. 
	PRE_SCENE_PASS_1, // 2nd pass in PRE_SCENE_PASS.
	PRE_SCENE_PASS_2, // 3rd pass in PRE_SCENE_PASS.
	SCENE_PASS_0,
	SCENE_PASS_1,
	SCENE_PASS_2,
	POST_PROCESS_0,   // After Light Pass(DirectionalLight, Spotlight, pointlights etc.)
	POST_PROCESS_1,
	POST_PROCESS_2,
	UI_LAYER_0,
	UI_LAYER_1,
	UI_LAYER_2,
	RENDER_PASS_COUNT
};

// Default FullscreenRenderer pipeline data
struct TexturePipelineData {
	std::wstring m_vertexShader = L"Shaders/FullscreenVS.cso";
	std::wstring m_pixelShader  = L"";

	uint16_t m_width  = WindowHandler::WindowData().m_w;
	uint16_t m_height = WindowHandler::WindowData().m_h;

	bool					   m_depthEnabled = false;
	D3D12_DEPTH_STENCIL_DESC   m_depthDesc    = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	DXGI_FORMAT				   m_depthFormat  = DXGI_FORMAT_UNKNOWN;

	D3D12_ROOT_SIGNATURE_FLAGS m_rsFlag		  = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	
	std::vector<DXGI_FORMAT>   m_format		  = {DXGI_FORMAT_R8G8B8A8_UNORM};	
	
	uint8_t					   m_rastDesc	  = 0;
	uint8_t					   m_blendDesc	  = 0;
	uint8_t					   m_samplerDesc  = 0;
	uint8_t					   m_inputLayout  = 0;
};

// TextureRenderingHandler handles most of the FullscreenTexture creations and all of it's rendering.
class TextureRenderingHandler
{
public:
	TextureRenderingHandler(D3D12Framework& framework, PSOHandler& psoHandler);
	~TextureRenderingHandler();

	// Creates a new FullscreenTexture and adds it directly to the pipeline
	FullscreenTexture* CreateFullscreenTexture(
		const std::wstring	 textureName,
		const uint16_t		 width, 
		const uint16_t		 height,
		DXGI_FORMAT			 srvFormat,
		RENDER_PASS			 transitionPoint,
		const uint16_t		 mipLevels = 1u,
		bool				 depthTexture = false
	);

	// Creates a new TextureRenderer and adds it directly to the pipeline
	TextureRenderer* CreateTextureRenderer(
		const std::wstring  name,
		TexturePipelineData data,
		const uint8_t	    numberOfBuffers,
		const uint8_t	    numberOfTextures,
		RENDER_PASS			renderPass,
		bool				renderAsDepth = false
	);
	// Creates a new TextureRenderer and adds it directly to the pipeline
	TextureRenderer* CreateTextureRenderer(
		const std::wstring		   name,
		std::wstring			   vertexShader,
		std::wstring			   pixelShader,
		const uint16_t			   viewportWidth,
		const uint16_t			   viewportHeight,
		bool					   useDepth,
		D3D12_DEPTH_STENCIL_DESC   depthDesc,
		DXGI_FORMAT				   depthFormat,
		std::vector<DXGI_FORMAT>   formats,
		const uint8_t			   blendDesc,
		const uint8_t			   rastDesc,
		const uint8_t			   samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAGS flags,
		const uint8_t			   numberOfBuffers,
		const uint8_t			   numberOfTextures,
		const uint8_t			   inputLayout,
		RENDER_PASS				   renderPass,
		bool					   renderAsDepth = false
	);

	// Adds a pre-existing FullscreenTexture to the pipeline and specified pass
	void AddFullscreenTextureToPipeline(FullscreenTexture* fullscreenTexture, RENDER_PASS transitionPoint);
	// Adds a pre-existing TextureRenderer to the pipeline and specified pass
	void AddTextureRendererToPipeline(TextureRenderer* fullscreenTexture, RENDER_PASS renderPass);

	const uint32_t DefaultBuffer()      { return m_defaultBufferOffset; } 
	const uint32_t DefaultLightBuffer() { return m_lightBufferOffset;   } 

	const FullscreenTexture* GetLastRenderedTexture() {
		return m_lastRenderedTexture;
	}

private:
	friend class RenderManager;

	void ClearAllTextures(ID3D12GraphicsCommandList* cmdList);
	void TransitionResourceForRendering();

	FullscreenTexture* FetchLastRenderedTexture();
	void RenderPass(RENDER_PASS from, RENDER_PASS to, ID3D12GraphicsCommandList* cmdList);

	void SetCommonBuffers(const uint32_t defaultBuffer, const uint32_t lightBuffer);

	uint8_t m_frameIndex;

	PSOHandler&		m_psoHandler; // Allows acces to RootSignatures & PiplelineStateObjects
	D3D12Framework& m_framework;

	uint32_t m_defaultBufferOffset;
	uint32_t m_lightBufferOffset;

	std::unordered_map<std::wstring, uint32_t> m_textureMap;

	std::array<std::vector<FullscreenTexture*>, RENDER_PASS_COUNT> m_textureList;
	std::array<std::vector<TextureRenderer*>,   RENDER_PASS_COUNT> m_renderList;
	
	FullscreenTexture* m_lastRenderedTexture = nullptr; // Currently a easy fix to always render the last texture to the back buffer, want to maybe in the future to have the actual back buffer in here instead.
};

