#pragma once

#include "DirectX/include/directx/d3dx12.h"
#include "Core/D3D12Header.h"

class PSOHandler;
class TextureHandler; 
class FullscreenTexture;
class ConstantBufferData;
class DescriptorHeapWrapper;

struct Texture;

class TextureRenderer
{
public:
	TextureRenderer() :
		m_viewPort({})
	{};

	void InitializeRenderer(
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
		PSOHandler&				   psoHandler,
		bool					   renderAsDepth = false
	);

	void SetTextureAtSlot(const FullscreenTexture* texture,		  const uint8_t slot, bool useRenderIndex = true);
	void SetTextureAtSlot(const Texture* texture,				  const uint8_t slot, bool useRenderIndex = true);
	void SetTextureAtSlot(const uint32_t srvHeapIndex,			  const uint8_t slot, bool useRenderIndex = true);
														    	  
	void SetBufferAtSlot(const ConstantBufferData* buffer,		  const uint8_t slot, bool useRenderIndex = true);
	void SetBufferAtSlot(const uint32_t cbvHeapIndex,			  const uint8_t slot, bool useRenderIndex = true);

	void SetRenderTargetAtSlot(const FullscreenTexture* texture,  const uint8_t slot, bool useRenderIndex = true);
	void SetRenderTargetAtSlot(const uint32_t rtvHeapIndex,		  const uint8_t slot, bool useRenderIndex = true);
	
	void SetDepthStencilView(const uint32_t dsvHeapIndex);

	const uint32_t GetPSO()			  { return m_pso;}
	const uint32_t GetRootSignature() { return m_rs; }
	const bool	   RenderToScreen()   { return m_renderToScreen; }
protected:
	friend class TextureRenderingHandler;

	// Main Function that renders to texture, returns either true or false to notify the TextureRenderHandler if it should draw to texture.
	virtual bool RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& heap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex);
	void PreparePipelineAndRootSignature(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler);
	void SetTextureAndBufferSlots(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex);

	std::vector<std::pair<uint32_t, bool>> m_textureSlots; // DescriptorHeap offset indicies with FrameIndex check at specified slot 
	std::vector<std::pair<uint32_t, bool>> m_bufferSlots;
	std::vector<std::pair<uint32_t, bool>> m_rtvSlots;

	uint32_t							   m_dsvHeapIndex = UINT32_MAX; // TODO: Implement so that TextureRenderers can draw to Depth
	uint32_t							   m_pso		  = UINT32_MAX;
	uint32_t							   m_rs			  = UINT32_MAX;

	std::wstring						   m_rendererName = L"";

	D3D12_VIEWPORT						   m_viewPort;
	
	//bool								   m_renderToLatestRTV = false;
	bool								   m_renderAsDepth     = false;
	bool								   m_renderToScreen    = false; // Notifies the TextureRendererHandler if it should render to texture. 
};

