#pragma once
#include "FullscreenShader.h"
#include "Rendering/Buffers/ConstantBufferData.h"

class KuwaharaFilter : FullScreenShader
{
public:
	KuwaharaFilter();
	~KuwaharaFilter();

	void InitializeFullscreenShader(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper) override;
	void SetPipelineAndRootsignature(PSOHandler& psoHandler) override;
	void SetFilterRadius(float radius);

protected:
	void UpdateBufferData(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* cbvWrapper, const UINT frameIndex) override;

private:
	struct Data {
		UINT m_width;
		UINT m_height;
		UINT m_radius;
	};

	Data m_data;
	ConstantBufferData m_cbvData;
};

