#include "stdafx.h"
#include "SSAOBuffer.h"

void SSAOBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_data = *data;
	memcpy(m_bufferGPUAddress[frame], &m_data, sizeof(Data));
}
