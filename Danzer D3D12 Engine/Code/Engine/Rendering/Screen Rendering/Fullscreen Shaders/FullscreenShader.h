#pragma once
#include "DirectX-Headers-main/include/directx/d3dx12.h"

class PSOHandler;
class DescriptorHeapWrapper;

class FullScreenShader
{
public:
	FullScreenShader();
	~FullScreenShader();
	
	void SetRenderTarget(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);
	
	void RenderEffect(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvSrvWrapper, const UINT frameIndex);
	void SetPSOandRS(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler);

	virtual void InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper);

	virtual void CreatePipelineAndRootsignature(PSOHandler& psoHandler) = 0;

protected:
	virtual void UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex);

	UINT m_pso, m_rs;
};

