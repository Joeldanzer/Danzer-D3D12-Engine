#include "stdafx.h"
#include "VolumetricLightBlur.h"
#include "Rendering/PSOHandler.h"

#include "Core/DesriptorHeapWrapper.h"

void VolumetricLightBlur::InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper)
{
	m_constantBuffer.IntializeBuffer(device, &cbvWrapper, sizeof(BufferData));
}

void VolumetricLightBlur::SetPipelineAndRootSignature(PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT); 
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(1, 2, PSOHandler::SAMPLER_CLAMP, flags, L"Volumetric Light Blur Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/FullscreenVS.cso", L"Shaders/VolumetricLightBlurPS.cso" },
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_NONE,
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::IL_NONE,
		L"Volumetric Light Blur PSO"
	);
}

void VolumetricLightBlur::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex)
{
	BufferData data = {
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		UINT(m_viewPort.Width  / 2),
		UINT(m_viewPort.Height / 2)
	};
	m_constantBuffer.UpdateBufferData(reinterpret_cast<UINT16*>(&data));
	m_constantBuffer.UpdateBufferToGPU(frameIndex);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbHandle = handle->GET_GPU_DESCRIPTOR(m_constantBuffer.OffsetID() + frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(0, cbHandle);

	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);

	cmdList->DrawInstanced(3, 1, 0, 0);
}
