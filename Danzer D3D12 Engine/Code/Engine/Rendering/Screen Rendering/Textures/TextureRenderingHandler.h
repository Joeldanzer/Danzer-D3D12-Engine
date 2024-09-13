#pragma once

#include "FullscreenTexture.h"

struct ID3D12GraphicsCommandList;

class DescriptorHeapWrapper;
class PSOHandler; 

// All the passes in the rendering pipeline, the order of first to last depending on each pass is 0 - 2 (start to end)
enum RENDER_PASS {
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

struct TexturePipelineData {
	std::wstring m_vertexShader			= L"Shaders/FullscreenVS.cso";;
	std::wstring m_pixelShader;

	bool m_depthEnabled					= false;
	DXGI_FORMAT m_format				= DXGI_FORMAT_R8G8B8A8_UNORM;	
	D3D12_ROOT_SIGNATURE_FLAGS m_rsFlag	= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	uint8_t m_rastDesc					= 0;
	uint8_t m_blendDesc					= 0;
	uint8_t m_samplerDesc				= 0;
	uint8_t m_inputLayout				= 0;
	DXGI_FORMAT m_depthFormat		    = DXGI_FORMAT_UNKNOWN;
};

// TextureRendering Handler handles most of the FullscreenTexture creations and all of it's rendering.
class TextureRenderingHandler
{
public:
	TextureRenderingHandler(D3D12Framework& framework, PSOHandler& psoHandler);
	~TextureRenderHandler();
	//void AddToRenderPass(FullscreenTexture* texture, RENDER_PASS pass); // Add already existing FullscreenTexture to render pass
	FullscreenTexture* CreateRenderTexture(
		const uint16_t		 width, 
		const uint16_t		 height,
		DXGI_FORMAT			 textureDesc,
		DXGI_FORMAT			 srvFormat,
		RENDER_PASS			 pass,
		TexturePipelineData  pipelineData,
		const uint8_t		 numberOfBuffers,
		const uint8_t		 numberOfTextures,
		std::wstring		 textureName
	);

	void AddFullscreenTextureToPipeline(FullscreenTexture* fullscreenTexture, RENDER_PASS renderPass);

	const uint32_t DefaultBuffer()      { return m_defaultBufferOffset; }
	const uint32_t DefaultLightBuffer() { return m_lightBufferOffset;   }

private:
	friend class RenderManager;

	void RenderToBackBuffer(ID3D12GraphicsCommandList* cmdList);
	void SetPSOHandler(PSOHandler* psoHandler);
	void SetCurrentFrameindex(const uint8_t index) { m_frameIndex = index; }
	void RenderPass(RENDER_PASS from, RENDER_PASS to, ID3D12GraphicsCommandList* cmdList);

	void SetCommonBuffers(const uint32_t defaultBuffer, const uint32_t lightBuffer);

	std::vector<ID3D12Resource*> FetchResources(RENDER_PASS from, RENDER_PASS to);

	uint8_t m_frameIndex;

	PSOHandler&		m_psoHandler;
	D3D12Framework& m_framework;

	uint32_t m_defaultBufferOffset;
	uint32_t m_lightBufferOffset;

	std::array<std::vector<FullscreenTexture*>, RENDER_PASS_COUNT> m_renderList;
};

