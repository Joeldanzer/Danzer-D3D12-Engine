#pragma once

#include "Components/Render & Effects/ModelEffect.h"
#include "ModelEffectData.h"
#include <vector>

class D3D12Framework;
class DescriptorHeapWrapper;
class PSOHandler;

class ModelEffectHandler
{	
public:
	ModelEffectHandler(D3D12Framework& framework, PSOHandler& psoHandler);
	~ModelEffectHandler();

	ModelEffect CreateModelEffect(
		std::array<std::wstring, 2> shaders,
		const UINT model, 
		void* bufferData,
		const UINT sizeOfData,
		std::vector<UINT> textures, 
		bool transparent
	);
	
	//ModelEffect GetModelEffect(std::wstring shaderName);
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
	
	PSOHandler* GetPSOHandler() {
		return m_psoHandler;
	}

private:	
	PSOHandler*     m_psoHandler;
	D3D12Framework* m_framework;
	ComPtr<ID3D12Resource> m_depthTexture;
	UINT m_depthTextureOffset;

	std::vector<ModelEffectData> m_modelEffects;
};

