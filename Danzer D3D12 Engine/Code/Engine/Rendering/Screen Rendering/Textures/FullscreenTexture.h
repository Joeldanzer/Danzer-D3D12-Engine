#pragma once
#include "DirectX/include/directx/d3dx12.h"
#include "Core/MathDefinitions.h"
#include "Core/D3D12Header.h"

class D3D12Framework;
class DescriptorHeapWrapper;
class TextureHandler;
class ConstantBufferData;
class PSOHandler;

struct Texture;

// Used for the rendering pipeline to 
class FullscreenTexture
{
public:
	FullscreenTexture() : 
		m_resourceState(D3D12_RESOURCE_STATE_COMMON),
		m_dsvOffsetID(0), 
		m_srvOffsetID(0),
		m_viewPort({}), 
		m_pso(0), 
		m_rs(0), 
		m_rtvOffsetID(0), 
		m_textureType(TextureType::RTV_TEXTURE)
	{}
	~FullscreenTexture();

	void InitAsDepth(
		ID3D12Device* device,
		DescriptorHeapWrapper* cbvSrvHeap,
		DescriptorHeapWrapper* dsvHeap,
		const UINT width,
		const UINT height,
		DXGI_FORMAT textureDesc,
		DXGI_FORMAT srvFormat,
		D3D12_RESOURCE_FLAGS flag,
		std::wstring name
	);
	void InitAsTexture(
		ID3D12Device* device,
		DescriptorHeapWrapper* cbvSrvHeap,
		DescriptorHeapWrapper* rtvHeap,
		const UINT width,
		const UINT height,
		DXGI_FORMAT textureDesc,
		DXGI_FORMAT srvFormat,
		D3D12_RESOURCE_FLAGS flag,
		std::wstring name
	);

	void SetPipelineAndRootSignature(
		std::wstring			   vertexShader,
		std::wstring			   pixelShader,
		bool					   depthEnabled,
		DXGI_FORMAT				   format,
		const uint8_t			   blendDesc,
		const uint8_t			   rastDesc,
		const uint8_t			   samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAGS flags,
		DXGI_FORMAT				   depthFormat,
		const uint8_t			   numberOfBuffers,
		const uint8_t			   numberOfTextures,
		const uint8_t			   inputLayout,
		std::wstring			   textureName,
		PSOHandler&				   psoHandler
	);
	
	void SetTextureAtSlot(const Texture*		   texture, const uint8_t slot, bool frameIndex = false);
	void SetTextureAtSlot(const FullscreenTexture* texture, const uint8_t slot, bool frameIndex = false);
	void SetTextureAtSlot(const uint32_t descriptorIndex,   const uint8_t slot, bool frameIndex = false);
	void SetBufferAtSlot(ConstantBufferData* buffer,		const uint8_t slot, bool frameIndex = false);
	void SetBufferAtSlot(const uint32_t descriptorIndex,	const uint8_t slot, bool frameIndex = false);

	void ClearTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvWrapper, const uint8_t frameIndex);

	void RenderTexture(bool render) {
		m_renderTexture = render;
	}

	ID3D12Resource* GetResource(const UINT frameIndex) {
		return m_resource[frameIndex].Get();
	}
	const D3D12_RESOURCE_STATES GetRenderingResourceState() {
		return m_resourceState;
	}
	const UINT SRVOffsetID() {
		return m_srvOffsetID;
	}
	const UINT DSVOffsetID() {
		return m_dsvOffsetID;
	}
	const UINT RTVOffsetID() {
		return m_rtvOffsetID;
	}
	const D3D12_VIEWPORT& GetViewPort() {
		return m_viewPort;
	}
	const UINT GetPSO() {
		return m_pso;
	}
	const UINT GetRootSignature() {
		return m_rs;
	}
	enum class TextureType {
		RTV_TEXTURE,
		DSV_TEXTURE
	};

protected:
	friend class TextureRenderingHandler;

	virtual void RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvWrapper, DescriptorHeapWrapper& cbvSrvWrapper, PSOHandler& psoHandler, const uint8_t frameIndex);
	void SetTextureAndBufferSlots(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& wrapper, const uint8_t frameIndex);

	TextureType m_textureType;

	bool m_renderTexture = true;

	uint32_t m_dsvOffsetID;
	uint32_t m_srvOffsetID; 
	uint32_t m_rtvOffsetID;

	std::vector<uint32_t> m_pipelineStates;
	std::vector<uint32_t> m_rootSignatures;

	uint32_t m_pso;
	uint32_t m_rs;

	std::vector<std::pair<uint32_t, bool>> m_textureSlots;
	std::vector<std::pair<uint32_t, bool>> m_bufferSlots;

	std::wstring m_resourceName = L"";
	D3D12_RESOURCE_STATES m_resourceState; // Keep track of what kind of a resource this texture is, used for clearing and transitioning the resource.
	D3D12_VIEWPORT m_viewPort;
	ComPtr<ID3D12Resource> m_resource[FrameCount];

private:
	bool m_textureIsInitialized = false;
};

