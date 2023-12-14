#pragma once
#include "DirectX-Headers-main/include/directx/d3dx12.h"
#include "Core/MathDefinitions.h"
#include "Core/D3D12Header.h"

class D3D12Framework;
class DescriptorHeapWrapper;

class DirectionalShadowMapping
{
public:
	DirectionalShadowMapping(
		ID3D12Device* device,
		DescriptorHeapWrapper* cbvSrvHeap,
		DescriptorHeapWrapper* dsvHeap,
		const UINT width,
		const UINT height,
		DXGI_FORMAT format
	);

	ID3D12Resource* GetResource(const UINT frameIndex) {
		return m_resource[frameIndex].Get();
	}

	const UINT SRVOffsetID(){
		return m_srvOffsetID;
	}
	const UINT DSVOffsetID() {
		return m_dsvOffsetID;
	}

	Mat4f& GetProjectionMatrix() {
		return m_projectionMatrix;
	}

	const D3D12_VIEWPORT& GetViewPort() {
		return m_viewPort;
	}

private:
	UINT m_dsvOffsetID;
	UINT m_srvOffsetID; //DescriptorHeapWrapper
	
	D3D12_VIEWPORT m_viewPort;

	Mat4f m_projectionMatrix;

	ComPtr<ID3D12Resource> m_resource[FrameCount];
};

