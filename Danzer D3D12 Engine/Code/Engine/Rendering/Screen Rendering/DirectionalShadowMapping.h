#pragma once
#include "TextureRenderer.h"

#include "Rendering/Models/ModelData.h"

class DirectionalShadowMapping : public TextureRenderer
{
public:	
	DirectionalShadowMapping() :
		m_models(nullptr), modelCount(0), m_projectionMatrix() 
	{}
	
	bool RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& dsvHandle, DescriptorHeapWrapper& cbvSrvHandle, const uint8_t frameIndex) override;
	void SetPipelineAndRootSignature(PSOHandler& psoHandler);

	void CreateProjection(float projectionScale, float increase);
	Mat4f& GetProjectionMatrix() {
		return m_projectionMatrix;
	}

	void SetModelsData(std::vector<ModelData>& models) {
		m_models   = &models[0];
		modelCount = uint32_t(models.size());
	}

private:
	uint32_t   modelCount;
	ModelData* m_models;

 	Mat4f m_projectionMatrix;
};

