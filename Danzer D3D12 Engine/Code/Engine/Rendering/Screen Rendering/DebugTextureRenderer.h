#pragma once
#include "TextureRenderer.h"
#include "Rendering/Buffers/VertexBuffer.h"

struct ID3D12PipelineState;

class DebugRenderingData;
class ModelHandler;
class ModelData;

class DebugTextureRenderer : public TextureRenderer
{
public:
	DebugTextureRenderer(ModelHandler& modelHandler);
	~DebugTextureRenderer();
	
	void InitializeDebugRenderer(
		const uint16_t			   viewportWidth,
		const uint16_t			   viewportHeight,
		std::vector<DXGI_FORMAT>   formats,
		PSOHandler&				   psoHandler
	);

	void SetRenderingData(DebugRenderingData* debugRenderingData) {
		m_debugData = debugRenderingData;
	}

protected:
	bool RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvHeap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex) override;

private:
	void RenderDebugAABBs(ID3D12GraphicsCommandList* cmdList, const uint32_t frameIndex);
	void RenderDebugLines(ID3D12GraphicsCommandList* cmdList, const uint32_t frameIndex);

	ID3D12PipelineState* m_linePSO;
	ID3D12PipelineState* m_aabbPSO;
	ID3D12PipelineState* m_spherePSO;

	DebugRenderingData*	 m_debugData;
	ModelHandler&		 m_modelHandler;
	uint32_t			 m_cubeID;
};

