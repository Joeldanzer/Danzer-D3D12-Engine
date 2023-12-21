#pragma once

#include "Components/ModelEffect.h"
#include "ModelEffectData.h"
#include <vector>

class D3D12Framework;

class ModelEffectHandler
{	
public:
	ModelEffectHandler(D3D12Framework& framework);
	~ModelEffectHandler();

	ModelEffect CreateModelEffect(
		std::wstring shaderNames,
		const UINT model, 
		const UINT numberOfBuffers, 
		std::vector<UINT> textures, 
		bool transparent
	);
	
private:	
	ID3D12Device* m_device;

	std::vector<ModelEffectData> m_modelEffects;
};

