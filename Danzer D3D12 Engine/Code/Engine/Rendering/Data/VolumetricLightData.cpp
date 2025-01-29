#include "stdafx.h"
#include "VolumetricLightData.h"

#include "Rendering/Buffers/BufferHandler.h"

void VolumetricLightData::InitializeConstantBuffer(BufferHandler& bufferHandler)
{
	m_constantBuffer = bufferHandler.CreateBufferData(sizeof(Data));
	m_constantBuffer->UpdateBufferData(reinterpret_cast<UINT16*>(&m_data));
}

void VolumetricLightData::SetNewData(const uint32_t steps, const float scattering, const float strength)
{
	m_data.m_numberOfSteps = steps;
	m_data.m_gScattering = scattering;
	m_data.m_scatteringStrength = strength;

	m_constantBuffer->UpdateBufferData(reinterpret_cast<UINT16*>(&m_data));
}
