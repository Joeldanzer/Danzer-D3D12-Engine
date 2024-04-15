#include "stdafx.h"
#include "GBuffer.h"

#include "Core/WindowHandler.h"
#include "Core/DesriptorHeapWrapper.h"
#include "Core/D3D12Framework.h"

#include "Rendering/PSOHandler.h"


GBuffer::GBuffer(D3D12Framework& framework, PSOHandler& psoHandler)
{
	InitializeGBuffers(framework, psoHandler);
}
GBuffer::~GBuffer(){}

std::array<ID3D12Resource*, GBUFFER_COUNT> GBuffer::GetGbufferResources()
{
	std::array<ID3D12Resource*, GBUFFER_COUNT> resources;
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
		resources[i] = m_resources[i].m_resource.Get();

	return resources;
}

std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> GBuffer::GetRTVDescriptorHandles(DescriptorHeapWrapper& rtvDesc)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(rtvDesc.GET_CPU_DESCRIPTOR(0));
	handle.Offset(m_rtvOffsetID * m_rtvDescSize);
	std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> rtvHandles;
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
	{
		rtvHandles[i] = handle;
		handle.Offset(1, m_rtvDescSize);
	}

	return rtvHandles;
}



void GBuffer::ClearRenderTargets(DescriptorHeapWrapper& rtvDesc, ID3D12GraphicsCommandList* cmdList, UINT numberOfRects, const D3D12_RECT* rect)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDesc.GET_CPU_DESCRIPTOR(0));
	rtvHandle.Offset(m_rtvOffsetID * m_rtvDescSize);
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
	{
		cmdList->ClearRenderTargetView(rtvHandle, ClearColor, numberOfRects, rect);
		rtvHandle.Offset(m_rtvDescSize);
	}
}

void GBuffer::AssignSRVSlots(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* srvWrapper, UINT& startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = srvWrapper->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = srvWrapper->DESCRIPTOR_SIZE();

	for (UINT i = 0; i < GBUFFER_COUNT; i++) {
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, m_resources[i].m_offsetID, cbvSrvDescSize);
		cmdList->SetGraphicsRootDescriptorTable(startLocation, srvHandle);
		startLocation++;
	}
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GetGPUHandle(GBUFFER_TEXTURES texture, DescriptorHeapWrapper* srvWrapper)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = srvWrapper->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = srvWrapper->DESCRIPTOR_SIZE();

	return CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, m_resources[texture].m_offsetID, cbvSrvDescSize);
}

void GBuffer::InitializeGBuffers(D3D12Framework& framework, PSOHandler& psoHandler)
{
	HRESULT result;

	ID3D12Device*   device	  = framework.GetDevice();
	IDXGISwapChain* swapChain = framework.GetSwapChain();

	UINT width, height;
	width  = WindowHandler::WindowData().m_w;
	height = WindowHandler::WindowData().m_h;

	 m_formats = {
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* ALBEDO
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* NORMAL
		DXGI_FORMAT_R8G8B8A8_UNORM,		 //* MATERIAL
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* VERTEX COLOR
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* VERTEX NORMAL
		DXGI_FORMAT_R32G32B32A32_FLOAT,  //* WORLD POSITION
		DXGI_FORMAT_R32_FLOAT			 //* DEPTH
	};

	InitPipelineAndRootSignature(psoHandler);

	std::array<std::wstring, GBUFFER_COUNT> bufferNames = {
		L"Albedo_Gbuffer",
		L"Normal_Gbuffer",
		L"Material_Gbuffer",
		L"Vertex_Color_Gbuffer",
		L"Vertex_Normal_Gbuffer",
		L"World_Position_Gbuffer",
		L"Depth_GBuffer"
	};
	

	m_rtvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_srvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	
	DescriptorHeapWrapper& srvWrapper = framework.CbvSrvHeap();
	DescriptorHeapWrapper& rtvWrapper = framework.RTVHeap();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvWrapper.GET_CPU_DESCRIPTOR(0));
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(framework.CbvSrvHeap().GET_CPU_DESCRIPTOR(0));

	srvHandle.Offset(srvWrapper.m_handleCurrentOffset * srvWrapper.DESCRIPTOR_SIZE());
	rtvHandle.Offset(rtvWrapper.m_handleCurrentOffset * rtvWrapper.DESCRIPTOR_SIZE());

	m_rtvOffsetID = rtvWrapper.m_handleCurrentOffset;

	for (UINT i = 0; i < GBUFFER_COUNT; i++)
	{
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			m_formats[i], width, height,
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);
		D3D12_CLEAR_VALUE clearValue;
		memcpy(&clearValue.Color[0], &ClearColor[0], sizeof(float) * 4);
		clearValue.Format = m_formats[i];

		result = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(m_resources[i].m_resource.GetAddressOf()));
		CHECK_HR(result);

		device->CreateRenderTargetView(m_resources[i].m_resource.Get(), nullptr, rtvHandle);
		rtvHandle.Offset(m_rtvDescSize);
		rtvWrapper.m_handleCurrentOffset++;

		m_resources[i].m_offsetID = srvWrapper.m_handleCurrentOffset;
		device->CreateShaderResourceView(m_resources[i].m_resource.Get(), nullptr, srvHandle);
		srvHandle.Offset(m_srvDescSize);
		srvWrapper.m_handleCurrentOffset++;

		m_resources[i].m_resource->SetName((LPCWSTR)bufferNames[i].c_str());
	}
}

void GBuffer::InitPipelineAndRootSignature(PSOHandler& psoHandler)
{
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	m_rs = psoHandler.CreateRootSignature(2, GBUFFER_COUNT, PSOHandler::SAMPLER_DESC_WRAP, flags, L"GBuffer Root Signature");

	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable	 = true;
	depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depth.DepthFunc		 = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depth.StencilEnable  = false;

	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/GbufferVS.cso", L"Shaders/GbufferPS.cso" }, 
		psoHandler.BlendDescs(PSOHandler::BLEND_DEFAULT),
		psoHandler.RastDescs(PSOHandler::RASTERIZER_FRONT),
		depth,
		DXGI_FORMAT_D32_FLOAT,
		&m_formats[0],
		GBUFFER_COUNT,
		m_rs,
		PSOHandler::INPUT_LAYOUT_INSTANCE_DEFFERED,
		L"GBuffer PSO"
	);

	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/GbufferVS.cso", L"Shaders/GbufferPS.cso" },
		psoHandler.BlendDescs(PSOHandler::BLEND_TRANSPARENT),
		psoHandler.RastDescs(PSOHandler::RASTERIZER_NONE),
		depth,
		DXGI_FORMAT_D32_FLOAT,
		&m_formats[0],
		GBUFFER_COUNT,
		m_rs,
		PSOHandler::INPUT_LAYOUT_INSTANCE_DEFFERED,
		L"GBuffer Transparent PSO"
	);

}
