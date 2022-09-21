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
		resources[i] = m_resources[i].Get();

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
		rtvHandle.Offset(1, m_rtvDescSize);
	}
}

void GBuffer::AssignSRVSlots(ID3D12GraphicsCommandList* cmdList, UINT startLocation)
{
	ID3D12DescriptorHeap* heap[1] = { m_srvDescriptor.Get() };
	cmdList->SetDescriptorHeaps(1, &heap[0]);
	cmdList->SetGraphicsRootDescriptorTable(startLocation, m_srvDescriptor->GetGPUDescriptorHandleForHeapStart());
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

	//for (UINT i = 0; i < GBUFFER_COUNT; i++)
	//{
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {  };
	rtvDesc.NumDescriptors = GBUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	
	result = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_rtvDescriptor.GetAddressOf()));
	//result = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_rtvDescriptor[i].GetAddressOf()));
	CHECK_HR(result);

	m_rtvDescriptor->SetName(L"Gbuffer RTV Descriptor");

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {  };
	srvDesc.NumDescriptors = GBUFFER_COUNT;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(m_srvDescriptor.GetAddressOf()));
	CHECK_HR(result);

	m_srvDescriptor->SetName(L"Gbuffer SRV Descriptor");

	float clearColor[4] = {0.5f, 0.5f, 1.f, 1.f};

	 m_rtvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	 m_srvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//m_rtvHeapSize[i] = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_srvDescriptor->GetCPUDescriptorHandleForHeapStart());

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
			IID_PPV_ARGS(m_resources[i].GetAddressOf()));
		CHECK_HR(result);
		
		device->CreateRenderTargetView(m_resources[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescSize);
		
		device->CreateShaderResourceView(m_resources[i].Get(), nullptr, srvHandle);
		srvHandle.Offset(1, m_srvDescSize);

		m_resources[i]->SetName((LPCWSTR)bufferNames[i].c_str());
	}




	//for (UINT j = 0; j < FrameCount; i++)
	//{			
	//	device->CreateRenderTargetView(m_resources[i][j].Get(), nullptr, rtvHandle);
	//	rtvHandle.Offset(1, rtvDescSize);
	//	
	//	device->CreateShaderResourceView(m_resources[i][j].Get(), nullptr, srvHandle);
	//	rtvHandle.Offset(1, srvDescSize);
	//
	//	std::wstring name(std::to_wstring(i));
	//	m_resources[i][j]->SetName((LPCWSTR)name.c_str());
	//}
	//}
}
