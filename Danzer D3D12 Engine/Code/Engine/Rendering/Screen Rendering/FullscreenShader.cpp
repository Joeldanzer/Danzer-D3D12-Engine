#include "stdafx.h"
#include "FullscreenShader.h"
#include "Core/D3D12Framework.h"


void FullScreenShader::SetRenderTarget(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
	cmdList->OMSetRenderTargets(rtvHandle ? 1 : 0, rtvHandle, false, dsvHandle);
}
