#include "stdafx.h"
#include "KuwaharaFilter.h"

#include "Core/D3D12Framework.h"
#include "Rendering/PSOHandler.h"
#include "Core/WindowHandler.h"

KuwaharaFilter::KuwaharaFilter() :
	m_data({Vect3f(), 0, 0, 0})
{
}

KuwaharaFilter::~KuwaharaFilter()
{
}

void KuwaharaFilter::InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper)
{	
	m_cbvData.IntializeBuffer(device, cbvWrapper, sizeof(Data));
}

void KuwaharaFilter::CreatePipelineAndRootsignature(PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			     D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS ;
	m_rs  = psoHandler.CreateRootSignature(1, 1, PSOHandler::SAMPLER_CLAMP, flags, L"Kuwahara Filter Root Signature");
	m_pso = psoHandler.CreateDefaultPSO(
		L"Shaders/FullscreenVS.cso", 
		L"Shaders/KuwaharaFilterPS.cso",
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_NONE,
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::IL_NONE,
		L"Kuwahara Filter PSO"
	);
}

void KuwaharaFilter::SetFilterRadius(UINT radius, UINT scale, Vect3f offset)
{
	m_data.offset   = offset;
	m_data.m_radius = radius;
	m_scale = scale;
}

void KuwaharaFilter::UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex)
{
	m_data.m_width  = WindowHandler::WindowData().m_w / m_scale;
	m_data.m_height = WindowHandler::WindowData().m_h / m_scale;

	m_cbvData.UpdateBufferData(reinterpret_cast<UINT16*>(&m_data));
	m_cbvData.UpdateBufferToGPU(frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(0, cbvWrapper->GET_GPU_DESCRIPTOR(m_cbvData.OffsetID() + frameIndex));
}

