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
		m_rtvOffsetID(0), 
		m_viewPort({})
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

	void ClearTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvWrapper, const uint8_t frameIndex);

	ID3D12Resource* GetResource(const uint8_t frameIndex) {
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

protected:
	friend class TextureRenderingHandler;

	bool m_renderTexture = true;

	uint32_t m_dsvOffsetID;
	uint32_t m_srvOffsetID; 
	uint32_t m_rtvOffsetID;

	D3D12_VIEWPORT m_viewPort;

	std::wstring		   m_resourceName = L"";
	D3D12_RESOURCE_STATES  m_resourceState; // Keep track of what kind of a resource this texture is, used for clearing and transitioning the resource.
	ComPtr<ID3D12Resource> m_resource[FrameCount];
};

