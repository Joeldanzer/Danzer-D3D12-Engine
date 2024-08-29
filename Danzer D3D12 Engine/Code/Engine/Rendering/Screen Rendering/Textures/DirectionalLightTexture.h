#pragma once
#include "FullscreenTexture.h"
#include "Components/Light/DirectionalLight.h"
#include "Rendering/Buffers/ConstantBufferData.h"

class  Camera;
class Transform;

class DirectionalLightTexture : public FullscreenTexture
{
public:
	DirectionalLightTexture();
	~DirectionalLightTexture();

	void InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper) override;

	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;

	void SetBufferData(const Mat4f& shadowProj, DirectionalLight& dirLight, Camera& cam, Transform& camTransform);

private:
	struct Data {
		Mat4f m_transform;
		Mat4f m_projection;
		Vect4f m_dir;
		Vect4f m_float4One;
		Vect4f m_float4Two;
		UINT   m_width;
		UINT   m_height;
	} m_lightData, m_cameraData;

	ConstantBufferData m_lightCbvData;
	ConstantBufferData m_camCbvData;
};

