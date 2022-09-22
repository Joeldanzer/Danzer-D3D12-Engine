#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class MaterialBuffer : public CBVBuffer
{
public:
	MaterialBuffer();
	~MaterialBuffer();

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT frameIndex, void* cbvData) override;

	// Camera Buffer Data
	struct Data {
		float m_metallic = 0.0f;
		float m_roughness = 0.0f;
		float m_emissive = 0.0f;
		// 0 = false, 1 = true
		UINT m_hasMaterialTexture = 0;
	};

private:
	Data m_materialBufferData;
};
