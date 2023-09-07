#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class MaterialBuffer : public CBVBuffer
{
public:
	MaterialBuffer();
	~MaterialBuffer();

	void Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper) override;
	void UpdateBuffer(UINT frameIndex, void* cbvData) override;

	// Camera Buffer Data
	struct Data {	
		float m_metallic = 0.0f;
		float m_roughness = 0.0f;
		float m_emissive = 0.0f;

		float m_color[4] = { 1.f, 1.f, 1.f, 1.f };
	};

private:
	Data m_materialBufferData;
};
