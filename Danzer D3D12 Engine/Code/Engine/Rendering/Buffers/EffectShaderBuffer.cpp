#include "stdafx.h"
#include "EffectShaderBuffer.h"

EffectShaderBuffer::EffectShaderBuffer()
{
}

EffectShaderBuffer::~EffectShaderBuffer()
{ 
}

void EffectShaderBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	//Data* data = reinterpret_cast<Data*>(cbvData);
	//m_data = *data;
	memcpy(&m_data, cbvData, m_sizeOfData);
	memcpy(m_bufferGPUAddress[frame], &m_data, sizeof(Data));
}
