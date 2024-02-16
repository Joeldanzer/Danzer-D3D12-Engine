#pragma once

#include "Components/ModelEffect.h"
#include "ModelEffectData.h"
#include <vector>

class D3D12Framework;
class DescriptorHeapWrapper;

class ModelEffectHandler
{	
public:
	ModelEffectHandler(D3D12Framework& framework);
	~ModelEffectHandler();

	ModelEffect CreateModelEffect(
		std::wstring shaderNames,
		const UINT model, 
		void* bufferData,
		const UINT sizeOfData,
		std::vector<UINT> textures, 
		bool transparent
	);
	
	ModelEffect GetModelEffect(std::wstring shaderName);
	ModelEffectData& GetModelEffectData(UINT effectID) {
		return m_modelEffects[effectID - 1];
	}

	std::vector<ModelEffectData>& GetAllEffects() {
		return m_modelEffects;
	}

	ID3D12Resource* GetDepthCopyTexture() {
		return m_depthTexture.Get();
	}

	const UINT GetDepthTextureOffset() {
		return m_depthTextureOffset;
	}

private:	
	D3D12Framework* m_framework;
	ComPtr<ID3D12Resource> m_depthTexture;
	UINT m_depthTextureOffset;

	std::vector<ModelEffectData> m_modelEffects;
};

