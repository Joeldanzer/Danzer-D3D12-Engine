#pragma once
#include "TextureRenderer.h"

class PSOHandler;
class LightHandler;

struct ID3D12PipelineState;

class LightRenderer : public TextureRenderer
{
public:
	LightRenderer(LightHandler& lightHandler) :
		m_lightHandler(lightHandler),
		m_pointLightPSO(nullptr),
		m_spotLightPSO(nullptr)
	{}
	~LightRenderer(){}

	void InitializeRenderer(PSOHandler& psoHandler, const uint32_t cbvCount, const uint32_t srvCount);
	
private:
	bool RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvHeap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex) override;

	LightHandler& m_lightHandler;

	ID3D12PipelineState* m_pointLightPSO;
	ID3D12PipelineState* m_spotLightPSO;
};

