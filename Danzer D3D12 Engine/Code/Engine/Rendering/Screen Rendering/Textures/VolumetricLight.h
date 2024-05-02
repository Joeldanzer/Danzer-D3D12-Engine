#pragma once
#include "FullscreenTexture.h"

#include "Rendering/Buffers/ConstantBufferData.h"

class Camera;
class DirectionalLight;

struct Transform;
struct DirectionalLight;
class  Camera;

class VolumetricLight : public FullscreenTexture
{
public:
	VolumetricLight();

	void InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper) override;
	void SetVolumetricData(const UINT steps, const float gScattering, const float scatteringStrength) { 
		m_volumetricData = { steps, gScattering, scatteringStrength };
	}
	void UpdateBufferData(Transform& camTransform, Camera& cam, DirectionalLight& directionalLight, const UINT frameIndex);

	void SetPipelineAndRootSignature(PSOHandler& psoHandler) override;
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;
	
	struct VolumetricData {
		UINT  m_numberOfSteps;
		float m_gScattering;
		float m_scatteringStrength;
	};
	
	const VolumetricData GetVolumetricData() {
		return m_volumetricData;
	}

private:
	struct CameraAndLightBuffer {
		Mat4f  m_view;
		Mat4f  m_projection;
		Vect4f m_float4;
		Vect4f m_color;
	};
	
	ConstantBufferData m_cameraBuffer;
	ConstantBufferData m_lightBuffer;
	ConstantBufferData m_volumetricLightBuffer;

	VolumetricData m_volumetricData;
};

