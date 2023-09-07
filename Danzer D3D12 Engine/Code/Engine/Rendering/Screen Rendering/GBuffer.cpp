#include "stdafx.h"
#include "GBuffer.h"

#include "Core/WindowHandler.h"

#include "Core/DirectX12Framework.h"


GBuffer::GBuffer(DirectX12Framework& framework)
{
	InitializeGBuffers(framework);
}

GBuffer::~GBuffer()
{
}


std::array<ID3D12Resource*, GBUFFER_COUNT> GBuffer::GetGbufferResources()
{
	std::array<ID3D12Resource*, GBUFFER_COUNT> resources;
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
		resources[i] = m_resources[i].m_resource.Get();

	return resources;
}

std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> GBuffer::GetRTVDescriptorHandles()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart());
	std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> rtvHandles;
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
	{
		rtvHandles[i] = handle;
		handle.Offset(1, m_rtvDescSize);
	}

	return rtvHandles;
}



void GBuffer::ClearRenderTargets(ID3D12GraphicsCommandList* cmdList, Vect4f clearColor, UINT numberOfRects, const D3D12_RECT* rect)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
	{
		cmdList->ClearRenderTargetView(rtvHandle, &clearColor.x, numberOfRects, rect);
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

void GBuffer::InitializeGBuffers(DirectX12Framework& framework)
{
	HRESULT result;

	ID3D12Device*   device	  = framework.GetDevice();
	IDXGISwapChain* swapChain = framework.GetSwapChain();

	UINT width, height;
	width  = WindowHandler::GetWindowData().m_width;
	height = WindowHandler::GetWindowData().m_height;

	std::array<DXGI_FORMAT, GBUFFER_COUNT> formats = {
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* ALBEDO
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* NORMAL
		DXGI_FORMAT_R8G8B8A8_UNORM,		 //* MATERIAL
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* VERTEX COLOR
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* VERTEX NORMAL
		DXGI_FORMAT_R32G32B32A32_FLOAT,  //* WORLD POSITION
	};

	std::array<std::wstring, GBUFFER_COUNT> bufferNames = {
		L"Albedo_Gbuffer",
		L"Normal_Gbuffer",
		L"Material_Gbuffer",
		L"Vertex_Color_Gbuffer",
		L"Vertex_Normal_Gbuffer",
		L"World_Position_Gbuffer"
	};

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {  };
	rtvDesc.NumDescriptors = GBUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	
	result = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_rtvDescriptor.GetAddressOf()));
	CHECK_HR(result);
	
	m_rtvDescriptor->SetName(L"Gbuffer RTV Descriptor");
	//
	//D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {  };
	//srvDesc.NumDescriptors = GBUFFER_COUNT;
	//srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//srvDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//
	//result = device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(m_srvDescriptor.GetAddressOf()));
	//CHECK_HR(result);

	//m_srvDescriptor->SetName(L"Gbuffer SRV Descriptor");
	m_rtvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_srvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	   
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(framework.GetCbvSrvUavWrapper().GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	float clearColor[4] = {0.f, 0.f, 0.f, 0.f};
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
	{
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			formats[i], width, height,
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);
		D3D12_CLEAR_VALUE clearValue;
		memcpy(&clearValue.Color[0], &clearColor[0], sizeof(float) * 4);
		clearValue.Format = formats[i];

		result = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(m_resources[i].m_resource.GetAddressOf()));
		CHECK_HR(result);

		device->CreateRenderTargetView(m_resources[i].m_resource.Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescSize);

		m_resources[i].m_offsetID = framework.GetCbvSrvUavWrapper().m_handleCurrentOffset;
		device->CreateShaderResourceView(m_resources[i].m_resource.Get(), nullptr, srvHandle);
		framework.GetCbvSrvUavWrapper().m_handleCurrentOffset += m_srvDescSize;
		srvHandle.Offset(m_srvDescSize);

		m_resources[i].m_resource->SetName((LPCWSTR)bufferNames[i].c_str());
	}
}
