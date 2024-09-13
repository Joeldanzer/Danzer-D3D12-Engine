#include "stdafx.h"
#include "FullscreenTexture.h"

#include "Core/DesriptorHeapWrapper.h"
#include "Rendering/PSOHandler.h"

#include "Rendering/TextureHandler.h"
#include "Rendering/Buffers/ConstantBufferData.h"
#include "Rendering/PSOHandler.h"

FullscreenTexture::~FullscreenTexture()
{
	for (size_t i = 0; i < FrameCount; i++)
		m_resource[i]->Release();	
}

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

	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(width), FLOAT(height));
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

	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(width), FLOAT(height));
}

void FullscreenTexture::SetPipelineAndRootSignature(std::wstring vertexShader, std::wstring pixelShader, bool depthEnabled, DXGI_FORMAT format, const uint8_t blendDesc, const uint8_t rastDesc, const uint8_t samplerDesc, D3D12_ROOT_SIGNATURE_FLAGS flags, DXGI_FORMAT depthFormat, const uint8_t numberOfBuffers, const uint8_t numberOfTextures, const uint8_t inputLayout, std::wstring textureName, PSOHandler& psoHandler)
{
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = depthEnabled;
	DXGI_FORMAT formats[] = { format };
	m_rs  = psoHandler.CreateRootSignature(numberOfBuffers, numberOfTextures, static_cast<PSOHandler::SAMPLER_DESCS>(samplerDesc), flags, textureName + L" Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ vertexShader, pixelShader },
		static_cast<PSOHandler::BLEND_DESC>(blendDesc),
		static_cast<PSOHandler::RASTERIZER_DESC>(rastDesc),
		depth,
		DXGI_FORMAT_UNKNOWN,
		&formats[0],
		_countof(formats),
		m_rs,
		static_cast<PSOHandler::INPUT_LAYOUTS>(inputLayout),
		textureName + L" PSO"
	);

	for (uint32_t i = 0; i < numberOfTextures; i++)
		m_textureSlots.push_back({ UINT32_MAX, true });

	for (uint32_t i = 0; i < numberOfBuffers; i++)
		m_bufferSlots.push_back({UINT32_MAX, true});
}

void FullscreenTexture::SetTextureAtSlot(const Texture* texture, const uint8_t slot, bool frameIndex)
{
	if (slot >= m_textureSlots.size()) {
		assert(slot >= m_textureSlots.size(), "Given Texture slot exceeds the number of defined slots!");
	}
	m_textureSlots[slot] = { texture->m_offsetID, frameIndex };
}

void FullscreenTexture::SetTextureAtSlot(const FullscreenTexture* texture, const uint8_t slot, bool frameIndex)
{
	if (slot >= m_textureSlots.size()) {
		assert(slot >= m_textureSlots.size(), "Given Texture slot exceeds the number of defined slots!");
	}
	m_textureSlots[slot] = { texture->m_srvOffsetID, frameIndex };
}

void FullscreenTexture::SetTextureAtSlot(const uint32_t descriptorIndex, const uint8_t slot, bool frameIndex)
{
	if (slot >= m_textureSlots.size()) {
		assert(slot >= m_textureSlots.size(), "Given Texture slot exceeds the number of defined slots!");
	}
	m_textureSlots[slot] = { descriptorIndex, frameIndex };
}

void FullscreenTexture::SetBufferAtSlot(ConstantBufferData* buffer, const uint8_t slot, bool frameIndex)
{
	if (slot >= m_bufferSlots.size()) {
		assert(slot >= m_bufferSlots.size(), "Given Buffer slot exceeds the number of defined slots!");
	}
	m_bufferSlots[slot] = { buffer->OffsetID(), frameIndex};
}

void FullscreenTexture::SetBufferAtSlot(const uint32_t descriptorIndex, const uint8_t slot, bool frameIndex)
{
	if (slot >= m_bufferSlots.size()) {
		assert(slot >= m_bufferSlots.size(), "Given Buffer slot exceeds the number of defined slots!");
	}
	m_bufferSlots[slot] = { descriptorIndex, frameIndex };
}

void FullscreenTexture::RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& wrapper, PSOHandler& psohandler, const uint8_t frameIndex)
{
	cmdList->SetGraphicsRootSignature(psohandler.GetRootSignature(m_rs));
	cmdList->SetPipelineState(psohandler.GetPipelineState(m_pso));

	cmdList->RSSetViewports(1, &m_viewPort);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = wrapper.GET_CPU_DESCRIPTOR((m_rtvOffsetID + frameIndex));
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	SetTextureAndBufferSlots(cmdList, wrapper, frameIndex);

	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetIndexBuffer(nullptr);

	cmdList->DrawInstanced(3, 1, 0, 0);
}

void FullscreenTexture::SetTextureAndBufferSlots(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& wrapper, const uint8_t frameIndex)
{
	uint16_t currentSlot = 0;

	for (size_t i = 0; i < m_bufferSlots.size(); i++)
	{

		if (m_bufferSlots[i].first != UINT32_MAX) {	
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle = wrapper.GET_GPU_DESCRIPTOR(m_bufferSlots[i].second ? m_bufferSlots[i].first + frameIndex : m_bufferSlots[i].first);
			cmdList->SetGraphicsRootDescriptorTable(currentSlot, handle);
		}
		currentSlot++;
	}

	for (size_t i = 0; i < m_textureSlots.size(); i++)
	{
		if (m_textureSlots[i].first != UINT32_MAX) {
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle = wrapper.GET_GPU_DESCRIPTOR(m_textureSlots[i].second ? m_textureSlots[i].first + frameIndex : m_textureSlots[i].first);
			cmdList->SetGraphicsRootDescriptorTable(currentSlot, handle);
		}
		
		currentSlot++;
	}
}
