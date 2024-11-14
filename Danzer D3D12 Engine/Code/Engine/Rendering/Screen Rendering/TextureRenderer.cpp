#include "stdafx.h"

#include "TextureRenderer.h"

#include "Rendering/PSOHandler.h"
#include "Rendering/Screen Rendering/Textures/FullscreenTexture.h"
#include "Rendering/Buffers/ConstantBufferData.h"
#include "Rendering/TextureHandler.h"

void TextureRenderer::InitializeRenderer(std::wstring vertexShader, std::wstring pixelShader, const uint16_t viewportWidth, const uint16_t viewportHeight, bool useDepth, D3D12_DEPTH_STENCIL_DESC depthDesc, DXGI_FORMAT depthFormat, std::vector<DXGI_FORMAT> formats, const uint8_t blendDesc, const uint8_t rastDesc, const uint8_t samplerDesc, D3D12_ROOT_SIGNATURE_FLAGS flags, const uint8_t numberOfBuffers, const uint8_t numberOfTextures, const uint8_t inputLayout, std::wstring name, PSOHandler& psoHandler, bool renderAsDepth)
{
	depthDesc.DepthEnable = useDepth;
	m_rs = psoHandler.CreateRootSignature(numberOfBuffers, numberOfTextures, static_cast<PSOHandler::SAMPLER_DESCS>(samplerDesc), flags, name + L" Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ vertexShader, pixelShader },
		static_cast<PSOHandler::BLEND_DESC>(blendDesc),
		static_cast<PSOHandler::RASTERIZER_DESC>(rastDesc),
		depthDesc,
		depthFormat,
		formats.empty() ? nullptr : &formats[0],
		formats.empty() ? 0		  : formats.size(),
		m_rs,
		static_cast<PSOHandler::INPUT_LAYOUTS>(inputLayout),
		name + L" PSO"
	);

	for (uint32_t i = 0; i < numberOfTextures; i++)
		m_textureSlots.push_back({ UINT32_MAX, true });

	for (uint32_t i = 0; i < numberOfBuffers; i++)
		m_bufferSlots.push_back({ UINT32_MAX, true });

	for (uint32_t i = 0; i < formats.size(); i++)
		m_rtvSlots.push_back({ UINT32_MAX, true });

	m_rendererName  = name;
	m_renderAsDepth = renderAsDepth;
	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(viewportWidth), FLOAT(viewportHeight));
}

void TextureRenderer::RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& heap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex)
{
	if (!m_renderAsDepth) {
		cmdList->RSSetViewports(1, &m_viewPort);

		std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
		rtvHandles.reserve(m_rtvSlots.size());
		for (size_t i = 0; i < m_rtvSlots.size(); i++) {
			if (m_rtvSlots[i].first != UINT32_MAX)
				rtvHandles.emplace_back(heap.GET_CPU_DESCRIPTOR((m_rtvSlots[i].second ? m_rtvSlots[i].first + frameIndex : m_rtvSlots[i].first)));
		}

		cmdList->OMSetRenderTargets(rtvHandles.size(), &rtvHandles[0], false, nullptr);

		cmdList->IASetVertexBuffers(0, 0, nullptr);
		cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetIndexBuffer(nullptr);

		cmdList->DrawInstanced(3, 1, 0, 0);
	}
	else {
		std::wstring errorMessage = L"WARNING: Resource '" + m_rendererName + L"' is a depth texture so RenderToTexture needs to be overridden!";
		OutputDebugString(errorMessage.c_str());
	}
}

void TextureRenderer::PreparePipelineAndRootSignature(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler)
{
	cmdList->SetGraphicsRootSignature(psoHandler.GetRootSignature(m_rs));
	cmdList->SetPipelineState(psoHandler.GetPipelineState(m_pso));
}

void TextureRenderer::SetTextureAndBufferSlots(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex)
{
	uint16_t currentSlot = 0;

	for (size_t i = 0; i < m_bufferSlots.size(); i++)
	{
		if (m_bufferSlots[i].first != UINT32_MAX) {
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle = cbvSrvHeap.GET_GPU_DESCRIPTOR(m_bufferSlots[i].second ? m_bufferSlots[i].first + frameIndex : m_bufferSlots[i].first);
			cmdList->SetGraphicsRootDescriptorTable(currentSlot, handle);
		}
		currentSlot++;
	}

	for (size_t i = 0; i < m_textureSlots.size(); i++)
	{
		if (m_textureSlots[i].first != UINT32_MAX) {
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle = cbvSrvHeap.GET_GPU_DESCRIPTOR(m_textureSlots[i].second ? m_textureSlots[i].first + frameIndex : m_textureSlots[i].first);
			cmdList->SetGraphicsRootDescriptorTable(currentSlot, handle);
		}
		currentSlot++;
	}
}

void TextureRenderer::SetTextureAtSlot(FullscreenTexture* texture, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_textureSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_textureSlots.size(), "Given Texture slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_textureSlots[slot] = { texture->SRVOffsetID(), useRenderIndex};
}

void TextureRenderer::SetTextureAtSlot(const Texture* texture, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_textureSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_textureSlots.size(), "Given Texture slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_textureSlots[slot] = { texture->m_offsetID, useRenderIndex };
}

void TextureRenderer::SetTextureAtSlot(const uint32_t srvHeapIndex, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_textureSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_textureSlots.size(), "Given Texture slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_textureSlots[slot] = { srvHeapIndex, useRenderIndex };
}

void TextureRenderer::SetBufferAtSlot(ConstantBufferData* buffer, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_bufferSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_bufferSlots.size(), "Given Buffer slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_bufferSlots[slot] = { buffer->OffsetID(), useRenderIndex};
}

void TextureRenderer::SetBufferAtSlot(const uint32_t cbvHeapIndex, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_bufferSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_bufferSlots.size(), "Given Buffer slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_bufferSlots[slot] = { cbvHeapIndex, useRenderIndex };
}

void TextureRenderer::SetRenderTargetAtSlot(FullscreenTexture* texture, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_rtvSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_bufferSlots.size(), "Given RenderTarget slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_rtvSlots[slot] = { texture->RTVOffsetID(), useRenderIndex };
}

void TextureRenderer::SetRenderTargetAtSlot(const uint32_t rtvHeapIndex, const uint8_t slot, bool useRenderIndex)
{
	if (slot >= m_rtvSlots.size()) {
#ifdef DEBUG
		assert(slot >= m_bufferSlots.size(), "Given RenderTarget slot exceeds the number of defined slots!");
#endif
		return;
	}
	m_rtvSlots[slot] = { rtvHeapIndex, useRenderIndex };
}

void TextureRenderer::SetDepthStencilView(const uint32_t dsvHeapIndex)
{
	m_dsvHeapIndex = dsvHeapIndex;
}
