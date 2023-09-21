#include "stdafx.h"
#include "LightBuffer.h"
#include "../../Core/DirectX12Framework.h"

LightBuffer::LightBuffer(){}
LightBuffer::~LightBuffer(){}

void LightBuffer::UpdateBuffer(void* cbvData)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_lightBufferData = *data;
	memcpy(m_bufferGPUAddress, &m_lightBufferData, sizeof(Data));
}