#pragma once
#include "Components/PointLight.h"
#include "Rendering/Buffers/PointLightBuffer.h"

class D3D12Framework;

class LightHandler
{
public:
	LightHandler(D3D12Framework& framework);
	~LightHandler();

	UINT CreatePointLight();

	//void UpdateLightBuffer(PointLight& light, const UINT frameIndex);
	PointLightBuffer& GetLightBuffer(const PointLight& light) {
		return m_pointLightBuffer[light.m_id];
	}

private:
	D3D12Framework& m_framework;
	std::vector<PointLightBuffer> m_pointLightBuffer;
};

