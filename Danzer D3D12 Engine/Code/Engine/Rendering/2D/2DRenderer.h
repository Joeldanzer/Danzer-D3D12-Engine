#pragma once
#include "SpriteData.h"
#include "Fonts\Font.h"

#include "../Buffers/WindowSizeBuffer.h"
#include "Rendering/TextureHandler.h"

#include <map>

struct ID3D12GraphicsCommandList;

class PSOHandler;
class D3D12Framework;
class SpriteHandler;

class Renderer2D
{
public:
	Renderer2D() : 
		m_framework(nullptr){}
	~Renderer2D();

	void Init(D3D12Framework& framework, PSOHandler& psoHandler);

	void UpdateDefaultUIBuffers(UINT frameIndex);
	void RenderUI(ID3D12GraphicsCommandList* cmdList, std::vector<SpriteData>& sprites, UINT frameIndex, std::vector<TextureHandler::Texture> m_textures);
	void RenderFontUI(ID3D12GraphicsCommandList* cmdList, std::vector<Font>& fonts, UINT frameIndex, std::vector<TextureHandler::Texture> m_textures);

	const UINT GetPSO() {
		return m_pso;
	}
	const UINT GetRootSignature() {
		return m_rs;
	}

private:
	void CreateUIVertexAndIndexBuffers(D3D12Framework& framework);

	D3D12Framework*         m_framework;
	
	UINT m_pso, m_rs;

	WindowBuffer				m_windowBuffer;
	WindowBuffer				m_spriteSheetBuffer; //Reuse Window Buffer for now until spritesheet needs something else

	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView = {};

	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_vertexBuffer;
};

