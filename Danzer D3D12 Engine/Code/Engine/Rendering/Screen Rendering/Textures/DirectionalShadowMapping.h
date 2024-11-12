#pragma once
#include "FullscreenTexture.h"

#include "Rendering/Models/ModelData.h"

class DirectionalShadowMapping : public FullscreenTexture
{
public:	
	DirectionalShadowMapping() :
		m_models(nullptr), modelCount(0), m_projectionMatrix() 
	{}
	
	void RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& dsvHandle, DescriptorHeapWrapper& cbvSrvHandle, PSOHandler& psoHandler, const uint8_t frameIndex) override;
	void SetPipelineAndRootSignature(PSOHandler& psoHandler);

	void CreateProjection(float projectionScale, float increase);
	Mat4f& GetProjectionMatrix() {
		return m_projectionMatrix;
	}

	void SetModelsData(std::vector<ModelData>& models) {
		m_models = &models[0];
		modelCount = UINT(models.size());
	}

private:
	UINT modelCount;
	ModelData* m_models;

 	Mat4f m_projectionMatrix;
};

