#include "stdafx.h"
#include "DirectionalLightTexture.h"
#include "Rendering/PSOHandler.h"
#include "Rendering/Screen Rendering/GBuffer.h"
#include "Core/WindowHandler.h"
#include "Core/DesriptorHeapWrapper.h"
#include "Components/Transform.h"
#include "Rendering/Camera.h"

DirectionalLightTexture::DirectionalLightTexture()
{
}

DirectionalLightTexture::~DirectionalLightTexture()
{
}

void DirectionalLightTexture::InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper)
{
	m_lightCbvData.IntializeBuffer(device, &cbvWrapper, sizeof(Data));
	m_camCbvData.IntializeBuffer(device, &cbvWrapper, sizeof(Data));
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
	m_camCbvData.UpdateBuffer(reinterpret_cast<UINT16*>(&m_cameraData), frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(0, handle->GET_GPU_DESCRIPTOR(m_camCbvData.OffsetID() + frameIndex));

	m_lightCbvData.UpdateBuffer(reinterpret_cast<UINT16*>(&m_lightData), frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(1, handle->GET_GPU_DESCRIPTOR(m_lightCbvData.OffsetID() + frameIndex));

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);

	cmdList->DrawInstanced(3, 1, 0, 0);
}

void DirectionalLightTexture::SetBufferData(const Mat4f& shadowProj, DirectionalLight& dirLight, Camera& cam, Transform& camTransform)
{
	Vect3f dir = dirLight.m_lightTransform.Forward();
	m_lightData = {
		DirectX::XMMatrixTranspose(dirLight.m_lightTransform.Invert()),
		DirectX::XMMatrixTranspose(shadowProj),
		dirLight.m_lightColor,
		dirLight.m_ambientColor,
		{dir.x, dir.y, dir.z, 1.0f },
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h
	};
	
	m_cameraData = {
		DirectX::XMMatrixTranspose(camTransform.World().Invert()),
		DirectX::XMMatrixTranspose(cam.GetProjection()),
		{camTransform.m_position.x, camTransform.m_position.y, camTransform.m_position.z, float(cam.RenderTarget())},
		{camTransform.World().Forward().x, camTransform.World().Forward().y, camTransform.World().Forward().z, 1.0f},
		{},
		0,
		0,
	};
}
