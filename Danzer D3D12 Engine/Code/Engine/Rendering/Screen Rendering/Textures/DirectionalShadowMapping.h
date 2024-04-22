#pragma once
#include "FullscreenTexture.h"

#include "Rendering/Models/ModelData.h"

class DirectionalShadowMapping : public FullscreenTexture
{
public:	
	DirectionalShadowMapping() :
		m_models(nullptr), modelCount(0), m_projectionMatrix() 
	{}
	
	void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex) override;
	void SetPipelineAndRootSignature(PSOHandler& psoHandler);

	void CreateProjection(float projectionScale, float increase);
	Mat4f& GetProjectionMatrix() {
		return m_projectionMatrix;
	}

	void SetModelsData(std::vector<ModelData>& models) {
		m_models = &models[0];
		modelCount = models.size();
	}

private:
	UINT modelCount;
	ModelData* m_models;

 	Mat4f m_projectionMatrix;
};

