#include "stdafx.h"
#include "VolumetricLight.h"

#include "Rendering/PSOHandler.h"
#include "Rendering/Camera.h"
#include "Components/Light/DirectionalLight.h"
#include "Components/Transform.h"

#include "Core/DesriptorHeapWrapper.h"

VolumetricLight::VolumetricLight(){}

void VolumetricLight::InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper)
{	
	m_cameraBuffer.IntializeBuffer(device, &cbvWrapper, sizeof(CameraAndLightBuffer));
	m_lightBuffer.IntializeBuffer(device, &cbvWrapper, sizeof(CameraAndLightBuffer));
	m_volumetricLightBuffer.IntializeBuffer(device, &cbvWrapper, sizeof(VolumetricData));
}

void VolumetricLight::UpdateBufferData(const Mat4f& shadowProj, Transform& camTransform, Camera& cam, DirectionalLight& directionalLight, const UINT frameIndex)
{
	Mat4f lightTransform = directionalLight.m_lightTransform;
	CameraAndLightBuffer camData;
	camData.m_view       = DirectX::XMMatrixTranspose(camTransform.World().Invert());
	camData.m_projection = DirectX::XMMatrixTranspose(cam.GetProjection());
	camData.m_float4 = { camTransform.m_position.x, camTransform.m_position.y, camTransform.m_position.z, 1.0f };
	m_cameraBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&camData), frameIndex);

	CameraAndLightBuffer lightData;
	lightData.m_view	   = DirectX::XMMatrixTranspose(lightTransform.Invert());
	lightData.m_projection = DirectX::XMMatrixTranspose(shadowProj);
	lightData.m_float4 = { lightTransform.Forward().x,  lightTransform.Forward().y, lightTransform.Forward().z, 1.0f};	
	lightData.m_color  = directionalLight.m_lightColor;
	m_lightBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&lightData), frameIndex);

	m_volumetricLightBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&m_volumetricData), frameIndex);
}

void VolumetricLight::SetPipelineAndRootSignature(PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		         D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(3, 2, PSOHandler::SAMPLER_DESC_CLAMP, flags, L"Volumetric Light Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/FullscreenVS.cso", L"Shaders/VolumetricLightingPS.cso" },
		CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::INPUT_LAYOUT_NONE,
		L"Volumetric Light PSO"
	);
}

void VolumetricLight::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE camHandle   = handle->GET_GPU_DESCRIPTOR(m_cameraBuffer.OffsetID() + frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE lightHandle = handle->GET_GPU_DESCRIPTOR(m_lightBuffer.OffsetID()  + frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE vlHandle    = handle->GET_GPU_DESCRIPTOR(m_volumetricLightBuffer.OffsetID()  + frameIndex);

	cmdList->SetGraphicsRootDescriptorTable(0, camHandle);
	cmdList->SetGraphicsRootDescriptorTable(1, lightHandle);
	cmdList->SetGraphicsRootDescriptorTable(2, vlHandle);

	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetIndexBuffer(nullptr);

	cmdList->DrawInstanced(3, 1, 0, 0);
}
