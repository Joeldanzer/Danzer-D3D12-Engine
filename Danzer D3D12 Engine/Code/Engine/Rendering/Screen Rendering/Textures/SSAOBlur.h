#pragma once
#include "FullscreenTextureOLD.h"

class SSAOBlur : public FullscreenTextureOLD
{
public:
	SSAOBlur();

	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;

private:
};

