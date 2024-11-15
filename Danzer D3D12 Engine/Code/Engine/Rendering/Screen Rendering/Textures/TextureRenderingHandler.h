#pragma once

#include "FullscreenTexture.h"

struct ID3D12GraphicsCommandList;

class TextureRenderer;
class DescriptorHeapWrapper;
class PSOHandler; 

// All the passes in the rendering pipeline, the order of first to last depending on each pass is 0 - 2 (start to end)
enum RENDER_PASS : uint8_t {
	NONE,
	PRE_SCENE_PASS_0, // 1st pass in PRE_SCENE_PASS. 
	PRE_SCENE_PASS_1, // 2nd pass in PRE_SCENE_PASS.
	PRE_SCENE_PASS_2, // 3rd pass in PRE_SCENE_PASS.
	SCENE_PASS_0,
	SCENE_PASS_1,
	SCENE_PASS_2,
	POST_PROCESS_0,   // After Light Pass(DirectionalLight, Spotlight, pointlights etc.)
	POST_PROCESS_1, 
	POST_PROCESS_2,
	RENDER_PASS_COUNT
};

// Default FullscreenTexture pipeline data, used for easily getting started.
struct TexturePipelineData {
	std::wstring m_vertexShader			= L"Shaders/FullscreenVS.cso";
	std::wstring m_pixelShader;

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

// TextureRendering Handler handles most of the FullscreenTexture creations and all of it's rendering.
class TextureRenderingHandler
{
public:
	TextureRenderingHandler(D3D12Framework& framework, PSOHandler& psoHandler);
	~TextureRenderingHandler();

	FullscreenTexture* CreateFullscreenTexture(
		const uint16_t		 width, 
		const uint16_t		 height,
		DXGI_FORMAT			 srvFormat,
		std::wstring		 textureName,
		RENDER_PASS			 transitionPoint,
		bool				 depthTexture = false
	);

	TextureRenderer* CreateTextureRenderer(
		TexturePipelineData data,
		const uint8_t	    numberOfBuffers,
		const uint8_t	    numberOfTextures,
		std::wstring		name,
		RENDER_PASS			renderPass,
		bool				renderAsDepth = false
	);
	TextureRenderer* CreateTextureRenderer(
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
		std::wstring			   name,
		RENDER_PASS				   renderPass,
		bool					   renderAsDepth = false
	);

	void AddFullscreenTextureToPipeline(FullscreenTexture* fullscreenTexture, RENDER_PASS transitionPoint);
	void AddTextureRendererToPipeline(TextureRenderer* fullscreenTexture, RENDER_PASS renderPass);

	const uint32_t DefaultBuffer()      { return m_defaultBufferOffset; }
	const uint32_t DefaultLightBuffer() { return m_lightBufferOffset;   }

private:
	friend class RenderManager;

	void ClearAllTextures(ID3D12GraphicsCommandList* cmdList);
	void TransitionResourceForRendering();

	FullscreenTexture* FetchLastRenderedTexture();
	void SetPSOHandler(PSOHandler* psoHandler);
	void SetCurrentFrameindex(const uint8_t index) { m_frameIndex = index; }
	void RenderPass(RENDER_PASS from, RENDER_PASS to, ID3D12GraphicsCommandList* cmdList);

	void SetCommonBuffers(const uint32_t defaultBuffer, const uint32_t lightBuffer);

	uint8_t m_frameIndex;

	PSOHandler&		m_psoHandler;
	D3D12Framework& m_framework;

	uint32_t m_defaultBufferOffset;
	uint32_t m_lightBufferOffset;

	std::unordered_map<std::wstring, uint32_t> m_textureMap;
	std::array<std::vector<FullscreenTexture*>, RENDER_PASS_COUNT> m_textureList;

	FullscreenTexture* m_lastRenderedTexture = nullptr;
	std::array<std::vector<TextureRenderer*>, RENDER_PASS_COUNT> m_renderList;
};

