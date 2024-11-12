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
		m_renderList[i].reserve(16);
	}
}

TextureRenderingHandler::~TextureRenderingHandler()
{
	for (uint8_t i = 0; i < RENDER_PASS_COUNT; i++)
	{
		for (uint16_t j = 0; j < m_renderList[i].size(); j++)
		{
			delete m_renderList[i][j];
			m_renderList[i][j] = nullptr;
		}
	}
}

FullscreenTexture* TextureRenderingHandler::CreateRenderTexture(const uint16_t width, const uint16_t height, DXGI_FORMAT textureDesc, DXGI_FORMAT srvFormat, RENDER_PASS pass, TexturePipelineData pipelineData, const uint8_t numberOfBuffers, const uint8_t numberOfTextures, std::wstring textureName)
{
	FullscreenTexture* texture = m_renderList[pass].emplace_back(new FullscreenTexture());

	texture->InitAsTexture(
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
	texture->SetPipelineAndRootSignature(
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
	
	return texture;
}

void TextureRenderingHandler::AddFullscreenTextureToPipeline(FullscreenTexture* fullscreenTexture, RENDER_PASS renderPass)
{
	m_renderList[renderPass].emplace_back(fullscreenTexture);
}

void TextureRenderingHandler::ClearAllTextures(ID3D12GraphicsCommandList* cmdList)
{
	for (uint8_t i = 0; i < RENDER_PASS_COUNT; i++)
	{
		for (uint8_t j = 0; j < m_renderList[i].size() ; j++)
		{
			m_renderList[i][j]->ClearTexture(cmdList, m_framework.RTVHeap(), m_frameIndex);
		}
	}
}

void TextureRenderingHandler::TransitionResourceForRendering()
{
	m_frameIndex = m_framework.GetFrameIndex();

	for (uint8_t i = 0; i < RENDER_PASS_COUNT; i++)
	{
		for (uint8_t j = 0; j < m_renderList[i].size(); j++)
		{
			m_framework.QeueuResourceTransition(m_renderList[i][j]->GetResource(m_frameIndex), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, m_renderList[i][j]->GetRenderingResourceState());
		}
	}
}

FullscreenTexture* TextureRenderingHandler::FetchLastRenderedTexture()
{
	return m_lastRenderedTexture;
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
		if (m_renderList[i].empty())
			continue;

		transitionResources.clear();
		for (uint16_t j = 0; j < m_renderList[i].size(); j++)
		{
			FullscreenTexture* texture = m_renderList[i][j];

			switch (texture->GetRenderingResourceState())
			{
			case D3D12_RESOURCE_STATE_DEPTH_WRITE:
				texture->RenderToTexture(cmdList, m_framework.DSVHeap(), m_framework.CbvSrvHeap(), m_psoHandler, m_frameIndex);
				break;
			case D3D12_RESOURCE_STATE_RENDER_TARGET:
				texture->RenderToTexture(cmdList, m_framework.RTVHeap(), m_framework.CbvSrvHeap(), m_psoHandler, m_frameIndex);
				break;
			}
			transitionResources.emplace_back(texture->GetResource(m_frameIndex));
			m_lastRenderedTexture = texture;

			m_framework.QeueuResourceTransition(texture->GetResource(m_frameIndex), texture->GetRenderingResourceState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		m_framework.TransitionAllResources();
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
			resources.emplace_back(m_renderList[i][j]->GetResource(m_framework.GetFrameIndex()));
		}
	}

	return resources;
}
