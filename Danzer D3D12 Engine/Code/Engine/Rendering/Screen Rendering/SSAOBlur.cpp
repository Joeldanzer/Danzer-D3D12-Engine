#include "stdafx.h"
#include "SSAOBlur.h"

#include "Rendering/PSOHandler.h"

SSAOBlur::SSAOBlur(){}

void SSAOBlur::SetPipelineAndRootSignature(PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R32_FLOAT };
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS			  |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS				  |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(1, 1, PSOHandler::SAMPLER_DESC_CLAMP, flags, L"SSAO Blur Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"FullscreenVS.cso", L"SSAOBlur.cso" },
		CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
		&format[0],
		1,
		m_rs,
		PSOHandler::INPUT_LAYOUT_NONE,
		L"SSAO Blur PSO"
	);
}
