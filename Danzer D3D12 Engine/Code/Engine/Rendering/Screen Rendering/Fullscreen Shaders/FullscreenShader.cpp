#include "stdafx.h"
#include "FullscreenShader.h"
#include "Core/D3D12Framework.h"


FullScreenShader::FullScreenShader() :
	m_pso(0), m_rs(0)
{
}

FullScreenShader::~FullScreenShader()
{
}

void FullScreenShader::InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper)
{
}

void FullScreenShader::SetRenderTarget(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
	cmdList->OMSetRenderTargets(rtvHandle ? 1 : 0, rtvHandle, false, dsvHandle);
}

void FullScreenShader::RenderEffect(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvSrvWrapper, const UINT frameIndex)
{
	if(cbvSrvWrapper)
		UpdateBufferData(cmdList, cbvSrvWrapper, frameIndex);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);

	cmdList->DrawInstanced(3, 1, 0, 0);
}

void FullScreenShader::UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex)
{

}

