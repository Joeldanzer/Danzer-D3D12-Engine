#include "stdafx.h"
#include "DirectionalLightTexture.h"
#include "Rendering/PSOHandler.h"
#include "Rendering/Screen Rendering/GBuffer.h"
#include "Core/WindowHandler.h"
#include "Core/DesriptorHeapWrapper.h"

DirectionalLightTexture::DirectionalLightTexture()
{
}

DirectionalLightTexture::~DirectionalLightTexture()
{
}

void DirectionalLightTexture::InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper)
{
	m_cbvData.IntializeBuffer(device, &cbvWrapper, sizeof(Data));
}

void DirectionalLightTexture::SetPipelineAndRootSignature(PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(2, GBUFFER_COUNT + 4, PSOHandler::SAMPLER_DESC_CLAMP, flags, L"Light Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/FullscreenVS.cso", L"Shaders/DirectionalLightPS.cso" },
		CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::INPUT_LAYOUT_NONE,
		L"Directional Light PSO"
	);
}

void DirectionalLightTexture::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex)
{
	m_cbvData.UpdateBuffer(reinterpret_cast<UINT16*>(&m_data), frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(0, handle->GET_GPU_DESCRIPTOR(m_cbvData.OffsetID() + frameIndex));

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);

	cmdList->DrawInstanced(3, 1, 0, 0);
}

void DirectionalLightTexture::SetBufferData(DirectionalLight& dirLight)
{
	Vect3f dir = dirLight.m_lightTransform.Forward();
	m_data = {
		dirLight.m_lightTransform,
		dirLight.m_lightProjection,
		{dir.x, dir.y, dir.z, 1.0f },
		dirLight.m_lightColor,
		dirLight.m_ambientColor,
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h
	};
}
