#pragma once
#include "FullscreenShader.h"
#include "Rendering/Buffers/ConstantBufferData.h"
#include "Core/MathDefinitions.h"

class KuwaharaFilter : public FullScreenShader
{
public:
	KuwaharaFilter();
	~KuwaharaFilter();

	void InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper) override;
	void CreatePipelineAndRootsignature(PSOHandler& psoHandler) override;
	void SetFilterRadius(UINT radius, UINT scale, Vect3f offset);

protected:
	void UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex) override;

private:
	struct Data {
		Vect3f offset;
		UINT m_radius;
		UINT m_width;
		UINT m_height;
	};

	UINT m_scale;

	Data m_data;
	ConstantBufferData m_cbvData;
};

