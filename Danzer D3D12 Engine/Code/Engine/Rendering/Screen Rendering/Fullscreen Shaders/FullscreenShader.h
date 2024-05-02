#pragma once
#include "DirectX-Headers-main/include/directx/d3dx12.h"

class PSOHandler;
class DescriptorHeapWrapper;

class FullScreenShader
{
public:
	FullScreenShader();
	~FullScreenShader();
	
	virtual void InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper);

	virtual void SetPipelineAndRootsignature(PSOHandler& psoHandler) = 0;

	void SetRenderTarget(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);
    void RenderEffect(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvSrvWrapper, const UINT frameIndex);

protected:
	virtual void UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex);

	UINT m_pso, m_rs;
};

