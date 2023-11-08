#include "stdafx.h"
#include "LightBuffer.h"
#include "../../Core/DirectX12Framework.h"

LightBuffer::LightBuffer(){}
LightBuffer::~LightBuffer(){}

void LightBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_lightBufferData = *data;
	memcpy(m_bufferGPUAddress[frame], &m_lightBufferData, sizeof(Data));
}