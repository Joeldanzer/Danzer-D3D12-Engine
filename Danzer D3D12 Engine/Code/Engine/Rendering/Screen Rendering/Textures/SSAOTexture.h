#pragma once
#include "FullscreenTexture.h"
#include "Rendering/Buffers/SSAOBuffer.h"
#include "Rendering/Buffers/WindowSizeBuffer.h"

class TextureHandler;
class D3D12Framework;

class SSAOTexture : public FullscreenTexture
{
public:
	SSAOTexture() : m_textureID(0), m_bufferOne(), m_bufferTwo()
	{}
	void InitializeSSAO(D3D12Framework& framework, TextureHandler& textureHandler, const UINT numberOfSamples, const UINT noiseSize);

	//void SetTextures(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& handle, TextureHandler& textureHandler);
	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;
	const UINT SampleTextureID() {
		return m_textureID;
	}
private:
	void SetBufferData(D3D12Framework& framework, const std::vector<Vect3f>& samples);

	SSAOBuffer          m_bufferOne;
	SSAOBuffer          m_bufferTwo;
	SSAOBuffer::Data    m_bufferDataOne;
	SSAOBuffer::Data    m_bufferDataTwo;
	WindowBuffer	    m_noiseScaleBuffer;
	WindowBuffer::Data  m_noiseScaleData;

	UINT m_textureID;
};

