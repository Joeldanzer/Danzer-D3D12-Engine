#pragma once
#include "FullscreenTextureOLD.h"
#include "Rendering/Buffers/ConstantBufferData.h"

class VolumetricLightBlur : public FullscreenTextureOLD
{
public:
	VolumetricLightBlur(){}
	~VolumetricLightBlur(){}

	void InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper) override;
	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;

private:
	struct BufferData {
		UINT  m_width;
		UINT  m_height;
		UINT  m_downScaledWidth;
		UINT  m_downScaledHeight;
	};
	ConstantBufferData m_constantBuffer;
};

