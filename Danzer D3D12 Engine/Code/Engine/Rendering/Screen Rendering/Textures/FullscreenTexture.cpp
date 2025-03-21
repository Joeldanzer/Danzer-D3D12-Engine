#include "stdafx.h"
#include "FullscreenTexture.h"

#include "Core/DesriptorHeapWrapper.h"
#include "Rendering/PSOHandler.h"

#include "Rendering/TextureHandler.h"
#include "Rendering/Buffers/ConstantBufferData.h"

FullscreenTexture::~FullscreenTexture()
{
	for (size_t i = 0; i < FrameCount; i++)
		m_resource[i]->Release();	
}

void FullscreenTexture::InitAsDepth(const std::wstring name, ID3D12Device* device, DescriptorHeapWrapper* cbvSrvHeap, DescriptorHeapWrapper* dsvHeap, const UINT width, const UINT height, DXGI_FORMAT textureDesc, DXGI_FORMAT srvFormat, D3D12_RESOURCE_FLAGS resourceFlag, const uint16_t mipLevels)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = cbvSrvHeap->GET_CPU_DESCRIPTOR(0);
	srvHandle.Offset(cbvSrvHeap->m_handleCurrentOffset * cbvSrvHeap->DESCRIPTOR_SIZE());

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GET_CPU_DESCRIPTOR(0);
	dsvHandle = dsvHeap->GET_CPU_DESCRIPTOR(0);
	dsvHandle.Offset(dsvHeap->m_handleCurrentOffset * dsvHeap->DESCRIPTOR_SIZE());

	m_dsvOffsetID = dsvHeap->m_handleCurrentOffset;
	m_srvOffsetID = cbvSrvHeap->m_handleCurrentOffset;

	cbvSrvHeap->m_handleCurrentOffset++;

	uint16_t mip = mipLevels > TextureHandler::MaxMipLevels ? TextureHandler::MaxMipLevels : mipLevels;
	mip = mipLevels <= 0 ? TextureHandler::MinMipLevels : mipLevels;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_RESOURCE_DESC desc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			width,
			height,
			mip,
			1,
			textureDesc,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			resourceFlag
		);


		D3D12_CLEAR_VALUE clearValue;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = srvFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		CHECK_HR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&m_resource[i])
		));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		device->CreateDepthStencilView(m_resource[i].Get(), &dsvDesc, dsvHandle);
		dsvHandle.Offset(dsvHeap->DESCRIPTOR_SIZE());
		dsvHeap->m_handleCurrentOffset++;

		device->CreateShaderResourceView(m_resource[i].Get(), &srvDesc, srvHandle);
		srvHandle.Offset(cbvSrvHeap->DESCRIPTOR_SIZE());
		cbvSrvHeap->m_handleCurrentOffset++;

		m_resource[i]->SetName(std::wstring(name + std::to_wstring(i)).c_str());
	}

	m_resourceName = name;
	m_resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(width), FLOAT(height));
}

void FullscreenTexture::InitAsTexture(const std::wstring name, ID3D12Device* device, DescriptorHeapWrapper* cbvSrvHeap, DescriptorHeapWrapper* rtvHeap, const UINT width, const UINT height, DXGI_FORMAT textureDesc, DXGI_FORMAT srvFormat, D3D12_RESOURCE_FLAGS resourceFlag, const uint16_t mipLevels)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = cbvSrvHeap->GET_CPU_DESCRIPTOR(0);
	srvHandle.Offset(cbvSrvHeap->m_handleCurrentOffset * cbvSrvHeap->DESCRIPTOR_SIZE());

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GET_CPU_DESCRIPTOR(0);
	rtvHandle.Offset(rtvHeap->m_handleCurrentOffset * rtvHeap->DESCRIPTOR_SIZE());

	m_rtvOffsetID = rtvHeap->m_handleCurrentOffset;
	m_srvOffsetID = cbvSrvHeap->m_handleCurrentOffset;

	uint16_t mip = mipLevels > TextureHandler::MaxMipLevels ? TextureHandler::MaxMipLevels : mipLevels;
	mip			 = mipLevels <= 0 ? TextureHandler::MinMipLevels : mipLevels;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_RESOURCE_DESC desc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			width,
			height,
			mip,
			1,
			textureDesc,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			resourceFlag
		);

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = srvFormat;
		memcpy(&clearValue.Color[0], &ClearColor[0], sizeof(float) * 4);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = srvFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		CHECK_HR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&m_resource[i])
		));

		device->CreateRenderTargetView(m_resource[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(rtvHeap->DESCRIPTOR_SIZE());
		rtvHeap->m_handleCurrentOffset++;

		device->CreateShaderResourceView(m_resource[i].Get(), &srvDesc, srvHandle);
		srvHandle.Offset(cbvSrvHeap->DESCRIPTOR_SIZE());
		cbvSrvHeap->m_handleCurrentOffset++;

		m_resource[i]->SetName(std::wstring(name + std::to_wstring(i)).c_str());
	}

	m_resourceName = name;
	m_resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(width), FLOAT(height));
}

void FullscreenTexture::ClearTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& heap, const uint8_t frameIndex)
{
	cmdList->RSSetViewports(1, &m_viewPort);
	switch (m_resourceState) {
	case D3D12_RESOURCE_STATE_DEPTH_WRITE:
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = heap.GET_CPU_DESCRIPTOR(m_dsvOffsetID + frameIndex);
		cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		break;
	case D3D12_RESOURCE_STATE_RENDER_TARGET:
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = heap.GET_CPU_DESCRIPTOR(m_rtvOffsetID + frameIndex);
		cmdList->ClearRenderTargetView(rtvHandle, &ClearColor[0], 0, nullptr);
		break;
	}
}