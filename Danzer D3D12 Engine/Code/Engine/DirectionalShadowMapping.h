#pragma once
#include "DirectX-Headers-main/include/directx/d3d12.h"
#include "Core/D3D12Header.h"

class D3D12Framework;


class DirectionalShadowMapping
{
public:
	DirectionalShadowMapping(
		D3D12Framework& framework, 
		const UINT width,
		const UINT height,
		DXGI_FORMAT format
		);

	//void RenderTexture();

private:
	UINT m_offsetID; //DescriptorHeapWrapper

	ComPtr<ID3D12Resource> m_resources[FrameCount];
};

