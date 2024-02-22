#include "stdafx.h"
#include "PointLightBuffer.h"

#include "../Engine/Core/D3D12Header.h"
#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

void PointLightBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	m_lightBufferData = *reinterpret_cast<Data*>(cbvData);
	memcpy(m_bufferGPUAddress[frame], &m_lightBufferData, sizeof(Data));
}
