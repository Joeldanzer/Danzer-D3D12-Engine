#pragma once
#include "Rendering/Screen Rendering/FullscreenTexture.h"

class SSAOBlur : public FullscreenTexture
{
public:
	SSAOBlur();

	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;

private:
};

