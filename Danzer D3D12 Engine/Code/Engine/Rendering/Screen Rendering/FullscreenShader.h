#pragma once
#include "DirectX-Headers-main/include/directx/d3dx12.h"

class PSOHandler;

class FullScreenShader
{
public:
	FullScreenShader();
	~FullScreenShader();

	virtual void SetPipelineAndRootsignature(PSOHandler& psoHandler) = 0;
	void SetRenderTarget(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);
};

