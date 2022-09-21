#pragma once
#include "SpriteData.h"
#include "Fonts\Font.h"

#include "../Buffers/WindowSizeBuffer.h"
#include "Rendering/TextureHandler.h"

#include <map>

struct ID3D12GraphicsCommandList;

class DirectX12Framework;
class SpriteHandler;

class Renderer2D
{
public:
	Renderer2D() : 
		m_commandList(nullptr) {}
	~Renderer2D();

	void Init(DirectX12Framework& framework);

	void UpdateDefaultUIBuffers(UINT frameIndex);
	void RenderUI(std::vector<SpriteData>& sprites, UINT frameIndex, std::vector<TextureHandler::Texture> m_textures);
	void RenderFontUI(std::vector<Font>& fonts, UINT frameIndex, std::vector<TextureHandler::Texture> m_textures);

private:
	void CreateUIVertexAndIndexBuffers(DirectX12Framework& framework);

	WindowBuffer				m_windowBuffer;
	WindowBuffer				m_spriteSheetBuffer; //Reuse Window Buffer for now until spritesheet needs something else

	ID3D12GraphicsCommandList*  m_commandList;

	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView = {};

	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_vertexBuffer;
};

