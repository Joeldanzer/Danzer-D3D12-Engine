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
	memcpy(m_bufferGPUAddress[frame], &cbvData, m_sizeOfData);
}
