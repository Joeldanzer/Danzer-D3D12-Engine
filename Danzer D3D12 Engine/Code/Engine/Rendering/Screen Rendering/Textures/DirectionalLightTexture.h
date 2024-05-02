#pragma once
#include "FullscreenTexture.h"
#include "Components/DirectionalLight.h"
#include "Rendering/Buffers/ConstantBufferData.h"

class DirectionalLightTexture : public FullscreenTexture
{
public:
	DirectionalLightTexture();
	~DirectionalLightTexture();

	void InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper) override;

	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;

	void SetBufferData(DirectionalLight& dirLight);

private:
	struct Data {
		Mat4f m_transform;
		Mat4f m_projection;
		Vect4f m_lightDir;
		Vect4f m_lightColor;
		Vect4f m_ambientColor;
		UINT   m_width;
		UINT   m_height;
	} m_data;

	ConstantBufferData m_cbvData;
};

