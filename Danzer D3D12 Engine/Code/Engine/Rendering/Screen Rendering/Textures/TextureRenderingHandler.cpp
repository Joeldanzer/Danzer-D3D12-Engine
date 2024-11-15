#include "stdafx.h"
#include "TextureRenderingHandler.h"

#include "Rendering/Screen Rendering/TextureRenderer.h"
#include "Rendering/PSOHandler.h"

TextureRenderingHandler::TextureRenderingHandler(D3D12Framework& framework, PSOHandler& psoHandler) :
	m_psoHandler(psoHandler),
	m_framework(framework),
	m_defaultBufferOffset(UINT32_MAX),
	m_lightBufferOffset(UINT32_MAX),
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

FullscreenTexture* TextureRenderingHandler::CreateFullscreenTexture(const uint16_t width, const uint16_t height, DXGI_FORMAT srvFormat, std::wstring textureName, RENDER_PASS transitionPoint, bool depthTexture)
{
	FullscreenTexture* texture = m_textureList[transitionPoint].emplace_back(new FullscreenTexture());
	//m_textureMap.emplace(textureName, m_textureList.size() - 1);

	if (!depthTexture) {
		texture->InitAsTexture(
			m_framework.GetDevice(),
			&m_framework.CbvSrvHeap(),
			&m_framework.RTVHeap(),
			width,
			height,
			srvFormat,
			srvFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			textureName
		);
	}
	else {
		texture->InitAsDepth(
			m_framework.GetDevice(),
			&m_framework.CbvSrvHeap(),
			&m_framework.DSVHeap(),
			width,
			height,
			DXGI_FORMAT_R32_TYPELESS,
			srvFormat,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			textureName
		);
	}

	return texture;
}

TextureRenderer* TextureRenderingHandler::CreateTextureRenderer(TexturePipelineData data, const uint8_t numberOfBuffers, const uint8_t numberOfTextures, std::wstring name, RENDER_PASS renderPass, bool renderAsDepth)
{
	TextureRenderer* renderer = m_renderList[renderPass].emplace_back(new TextureRenderer());

	renderer->InitializeRenderer(
		data.m_vertexShader,
		data.m_pixelShader,
		data.m_width,
		data.m_height,
		data.m_depthEnabled,
		data.m_depthDesc,
		data.m_depthFormat,
		data.m_format,
		data.m_blendDesc,
		data.m_rastDesc,
		data.m_samplerDesc,
		data.m_rsFlag,
		numberOfBuffers,
		numberOfTextures,
		data.m_inputLayout,
		name,
		m_psoHandler,
		renderAsDepth
	);

	return renderer;
}

TextureRenderer* TextureRenderingHandler::CreateTextureRenderer(std::wstring vertexShader, std::wstring pixelShader, const uint16_t viewportWidth, const uint16_t viewportHeight, bool useDepth, D3D12_DEPTH_STENCIL_DESC depthDesc, DXGI_FORMAT depthFormat, std::vector<DXGI_FORMAT> formats, const uint8_t blendDesc, const uint8_t rastDesc, const uint8_t samplerDesc, D3D12_ROOT_SIGNATURE_FLAGS flags, const uint8_t numberOfBuffers, const uint8_t numberOfTextures, const uint8_t inputLayout, std::wstring name, RENDER_PASS renderPass, bool renderAsDepth)
{
	TextureRenderer* renderer = m_renderList[renderPass].emplace_back(new TextureRenderer());
	
	renderer->InitializeRenderer(
		vertexShader,
		pixelShader,
		viewportWidth,
		viewportHeight,
		useDepth,
		depthDesc,
		depthFormat,
		formats,
		blendDesc,
		rastDesc,
		samplerDesc,
		flags,
		numberOfBuffers,
		numberOfTextures,
		inputLayout,
		name,
		m_psoHandler,
		renderAsDepth
	);

	return renderer;
}

void TextureRenderingHandler::AddFullscreenTextureToPipeline(FullscreenTexture* fullscreenTexture, RENDER_PASS transitionPoint)
{
	if (fullscreenTexture && transitionPoint != RENDER_PASS_COUNT) {
		m_textureList[transitionPoint].emplace_back(fullscreenTexture);
		//m_textureMap.emplace(fullscreenTexture->m_resourceName, m_textureList.size() - 1);
	}
}

void TextureRenderingHandler::AddTextureRendererToPipeline(TextureRenderer* fullscreenTexture, RENDER_PASS renderPass)
{
	if(fullscreenTexture && renderPass != RENDER_PASS_COUNT)
		m_renderList[renderPass].push_back(fullscreenTexture);
}

void TextureRenderingHandler::ClearAllTextures(ID3D12GraphicsCommandList* cmdList)
{
	for (uint8_t i = 0; i < m_textureList.size(); i++)
	{
		for (uint32_t j = 0; j < m_textureList[i].size(); j++)
		{
			switch (m_textureList[i][j]->GetRenderingResourceState()) {
			case D3D12_RESOURCE_STATE_DEPTH_WRITE:
				m_textureList[i][j]->ClearTexture(cmdList, m_framework.DSVHeap(), m_frameIndex);
				break;
			case D3D12_RESOURCE_STATE_RENDER_TARGET:
				m_textureList[i][j]->ClearTexture(cmdList, m_framework.RTVHeap(), m_frameIndex);
				break;
			}
		}	
	}
}

void TextureRenderingHandler::TransitionResourceForRendering()
{
	m_frameIndex = m_framework.GetFrameIndex();
	for (uint8_t i = 0; i < m_textureList.size(); i++)
	{
		for (uint32_t j = 0; j < m_textureList[i].size(); j++)
			m_framework.QeueuResourceTransition(m_textureList[i][j]->GetResource(m_frameIndex), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, m_textureList[i][j]->GetRenderingResourceState());
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
		for (uint16_t j = 0; j < m_renderList[i].size(); j++)
		{
			TextureRenderer* renderer = m_renderList[i][j];

			renderer->PreparePipelineAndRootSignature(cmdList, m_psoHandler);
			renderer->SetTextureAndBufferSlots(cmdList, m_framework.CbvSrvHeap(), m_frameIndex);

			if(renderer->m_renderAsDepth)
				renderer->RenderToTexture(cmdList, m_framework.DSVHeap(), m_framework.CbvSrvHeap(), m_frameIndex);
			else
				renderer->RenderToTexture(cmdList, m_framework.RTVHeap(), m_framework.CbvSrvHeap(), m_frameIndex);
		}
		
		if (m_textureList[i].empty())
			continue;

		for (uint32_t j = 0; j < m_textureList[i].size(); j++)
		{
			m_framework.QeueuResourceTransition(m_textureList[i][j]->GetResource(m_frameIndex), m_textureList[i][j]->GetRenderingResourceState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			m_lastRenderedTexture = m_textureList[i][j];
		}
		m_framework.TransitionAllResources();
	}
}

void TextureRenderingHandler::SetCommonBuffers(const uint32_t defaultBuffer, const uint32_t lightBuffer)
{
	m_defaultBufferOffset = defaultBuffer;
	m_lightBufferOffset   = lightBuffer;
}
