#include "stdafx.h"
#include "TextureRenderingHandler.h"

#include "Rendering/PSOHandler.h"

TextureRenderingHandler::TextureRenderingHandler(D3D12Framework& framework, PSOHandler& psoHandler) :
	m_psoHandler(psoHandler),
	m_framework(framework),
	m_frameIndex(0)
{
	for (uint8_t i = PRE_SCENE_PASS_0; i < RENDER_PASS_COUNT; i++)
	{
		m_renderList[i].reserve(256);
	}
}

FullscreenTexture* TextureRenderingHandler::CreateRenderTexture(const uint16_t width, const uint16_t height, DXGI_FORMAT textureDesc, DXGI_FORMAT srvFormat, RENDER_PASS pass, TexturePipelineData pipelineData, const uint8_t numberOfBuffers, const uint8_t numberOfTextures, std::wstring textureName)
{
	FullscreenTexture& texture = m_renderList[pass].emplace_back();

	texture.InitAsTexture(
		m_framework.GetDevice(),
		&m_framework.CbvSrvHeap(),
		&m_framework.RTVHeap(),
		width,
		height,
		textureDesc,
		srvFormat,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		textureName
	);
	texture.SetPipelineAndRootSignature(
		pipelineData.m_vertexShader,
		pipelineData.m_pixelShader,
		pipelineData.m_depthEnabled,
		srvFormat,
		pipelineData.m_blendDesc,
		pipelineData.m_rastDesc,
		pipelineData.m_samplerDesc,
		pipelineData.m_rsFlag,
		pipelineData.m_depthFormat,
		numberOfBuffers,
		numberOfTextures,
		pipelineData.m_inputLayout,
		textureName,
		m_psoHandler
	);
	
	return &texture;
}

void TextureRenderingHandler::SetPSOHandler(PSOHandler* psoHandler)
{
	//m_psoHandler = psoHandler;
}

void TextureRenderingHandler::RenderPass(RENDER_PASS from, RENDER_PASS to, ID3D12GraphicsCommandList* cmdList)
{ 
	std::vector<ID3D12Resource*> transitionResources;
	for (uint8_t i = uint8_t(from); i < uint8_t(to) + 1; i++)
	{
		transitionResources.clear();

		for (uint16_t j = 0; j < m_renderList[i].size(); j++)
		{
			FullscreenTexture& texture = m_renderList[i][j];

			cmdList->SetGraphicsRootSignature(m_psoHandler.GetRootSignature(texture.m_rs));
			cmdList->SetPipelineState(m_psoHandler.GetPipelineState(texture.m_pso));

			cmdList->RSSetViewports(1, &texture.m_viewPort);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_framework.RTVHeap().GET_CPU_DESCRIPTOR((texture.m_rtvOffsetID + m_frameIndex));
			cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
			
			texture.SetTextureAndBufferSlots(cmdList, m_framework.CbvSrvHeap(), m_frameIndex);
			
			cmdList->IASetVertexBuffers(0, 0, nullptr);
			cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdList->IASetIndexBuffer(nullptr);

			cmdList->DrawInstanced(3, 1, 0, 0);

			transitionResources.emplace_back(texture.GetResource(m_frameIndex));
		}

		// Transition render targets to pixel shader for use. 
		if (!transitionResources.empty()) {
			m_framework.QeueuResourceTransition(&transitionResources[0], transitionResources.size(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			m_framework.TransitionAllResources();
		}
	}
}

void TextureRenderingHandler::SetCommonBuffers(const uint32_t defaultBuffer, const uint32_t lightBuffer)
{
	m_defaultBufferOffset = defaultBuffer;
	m_lightBufferOffset   = lightBuffer;
}

std::vector<ID3D12Resource*> TextureRenderingHandler::FetchResources(RENDER_PASS from, RENDER_PASS to)
{
	std::vector<ID3D12Resource*> resources;
	for (uint8_t i = uint8_t(from); i < uint8_t(to) + 1; i++)
	{
		for (uint16_t j = 0; j < m_renderList[i].size(); j++)
		{
			FullscreenTexture& texture = m_renderList[i][j];
			resources.emplace_back(texture.GetResource(m_frameIndex));
		}
	}

	return resources;
}
