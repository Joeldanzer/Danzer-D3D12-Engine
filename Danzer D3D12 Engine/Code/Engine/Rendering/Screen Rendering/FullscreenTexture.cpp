#include "stdafx.h"
#include "FullscreenTexture.h"

#include "Core/DesriptorHeapWrapper.h"

void FullscreenTexture::InitAsDepth(ID3D12Device* device, DescriptorHeapWrapper* cbvSrvHeap, DescriptorHeapWrapper* dsvHeap, const UINT width, const UINT height, DXGI_FORMAT textureDesc, DXGI_FORMAT srvFormat, D3D12_RESOURCE_FLAGS flag, std::wstring name)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = cbvSrvHeap->GET_CPU_DESCRIPTOR(0);
	srvHandle.Offset(cbvSrvHeap->m_handleCurrentOffset * cbvSrvHeap->DESCRIPTOR_SIZE());

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GET_CPU_DESCRIPTOR(0);
	dsvHandle = dsvHeap->GET_CPU_DESCRIPTOR(0);
	dsvHandle.Offset(dsvHeap->m_handleCurrentOffset * dsvHeap->DESCRIPTOR_SIZE());

	m_dsvOffsetID = dsvHeap->m_handleCurrentOffset;
	m_srvOffsetID = cbvSrvHeap->m_handleCurrentOffset;

	cbvSrvHeap->m_handleCurrentOffset++;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_RESOURCE_DESC desc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			width,
			height,
			1,
			1,
			textureDesc,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			flag
		);

		D3D12_CLEAR_VALUE clearValue;
		clearValue.DepthStencil.Depth   = 1.0f;	
		clearValue.DepthStencil.Stencil = 0.0f;	
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

	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
}

void FullscreenTexture::InitAsTexture(ID3D12Device* device, DescriptorHeapWrapper* cbvSrvHeap, DescriptorHeapWrapper* rtvHeap, const UINT width, const UINT height, DXGI_FORMAT textureDesc, DXGI_FORMAT srvFormat, D3D12_RESOURCE_FLAGS flag, std::wstring name)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = cbvSrvHeap->GET_CPU_DESCRIPTOR(0);
	srvHandle.Offset(cbvSrvHeap->m_handleCurrentOffset * cbvSrvHeap->DESCRIPTOR_SIZE());
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GET_CPU_DESCRIPTOR(0);
	rtvHandle.Offset(rtvHeap->m_handleCurrentOffset * rtvHeap->DESCRIPTOR_SIZE());
	
	m_rtvOffsetID = rtvHeap->m_handleCurrentOffset;
	m_srvOffsetID = cbvSrvHeap->m_handleCurrentOffset;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_RESOURCE_DESC desc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			width,
			height,
			1,
			1,
			textureDesc,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			flag
		);

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = srvFormat;
		memcpy(&clearValue.Color[0], &ClearColor[0], sizeof(float) * 4);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = srvFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1; 
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

	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
}

void FullscreenTexture::SetTextureAtSlot(ID3D12GraphicsCommandList* cmdList, const UINT slot, D3D12_GPU_DESCRIPTOR_HANDLE handle)
{
	cmdList->SetGraphicsRootDescriptorTable(slot, handle);
}
