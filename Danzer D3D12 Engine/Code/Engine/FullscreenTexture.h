#pragma once
#include "DirectX-Headers-main/include/directx/d3dx12.h"
#include "Core/MathDefinitions.h"
#include "Core/D3D12Header.h"

class D3D12Framework;
class DescriptorHeapWrapper;

class FullscreenTexture
{
public:
	FullscreenTexture() : 
		m_dsvOffsetID(0), m_srvOffsetID(0), m_viewPort({}) 
	{}

	void InitTexture(
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

	virtual void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& handle, const UINT frameIndex) = 0;

	ID3D12Resource* GetResource(const UINT frameIndex) {
		return m_resource[frameIndex].Get();
	}

	const UINT SRVOffsetID() {
		return m_srvOffsetID;
	}
	const UINT DSVOffsetID() {
		return m_dsvOffsetID;
	}
	const D3D12_VIEWPORT& GetViewPort() {
		return m_viewPort;
	}

protected:
	UINT m_dsvOffsetID;
	UINT m_srvOffsetID; //DescriptorHeapWrapper

	D3D12_VIEWPORT m_viewPort;
	ComPtr<ID3D12Resource> m_resource[FrameCount];
};
