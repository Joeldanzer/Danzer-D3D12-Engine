#include "stdafx.h"
#include "SSAOBlur.h"

#include "Rendering/PSOHandler.h"

SSAOBlur::SSAOBlur(){}

void SSAOBlur::SetPipelineAndRootSignature(PSOHandler& psoHandler)
{
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	DXGI_FORMAT format[] = { DXGI_FORMAT_R32_FLOAT };
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS			  |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS				  |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(0, 1, PSOHandler::SAMPLER_CLAMP, flags, L"SSAO Blur Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/FullscreenVS.cso", L"Shaders/SSAOBlurPS.cso" },
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_BACK,
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		_countof(format),
		m_rs,
		PSOHandler::IL_NONE,
		L"SSAO Blur PSO"
	);
}

void SSAOBlur::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex)
{
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawInstanced(3, 1, 0, 0);
}
