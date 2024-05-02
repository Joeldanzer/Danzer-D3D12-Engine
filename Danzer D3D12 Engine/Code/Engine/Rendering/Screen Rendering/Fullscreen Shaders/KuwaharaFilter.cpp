#include "stdafx.h"
#include "KuwaharaFilter.h"

#include "Core/D3D12Framework.h"
#include "Rendering/PSOHandler.h"

KuwaharaFilter::KuwaharaFilter()
{
}

KuwaharaFilter::~KuwaharaFilter()
{
}

void KuwaharaFilter::InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper)
{	
	m_cbvData.IntializeBuffer(device, cbvWrapper, sizeof(Data));
}

void KuwaharaFilter::SetPipelineAndRootsignature(PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			     D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS ;
	m_rs  = psoHandler.CreateRootSignature(1, 1, PSOHandler::SAMPLER_DESC_CLAMP, flags, L"Kuwahara Filter Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/FullscreenVS.cso", L"Shaders/KuwaharaFilterPS.cso" },
		CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::INPUT_LAYOUT_NONE,
		L"Kuwahara Filter PSO"
	);
}

void KuwaharaFilter::SetFilterRadius(float radius)
{
	m_data.m_radius = radius;
}

void KuwaharaFilter::UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex)
{
	m_cbvData.UpdateBuffer(reinterpret_cast<UINT16*>(&m_cbvData), frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(0, cbvWrapper->GET_GPU_DESCRIPTOR(m_cbvData.OffsetID() + frameIndex));	
}

