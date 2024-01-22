#include "stdafx.h"
#include "PointLightBuffer.h"

void PointLightBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_lightBufferData = *data;
	memcpy(m_bufferGPUAddress[frame], &m_lightBufferData, sizeof(Data));
}
