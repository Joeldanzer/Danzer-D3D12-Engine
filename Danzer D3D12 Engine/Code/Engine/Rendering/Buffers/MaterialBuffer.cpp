#include "stdafx.h"
#include "MaterialBuffer.h"

#include "../../Core/DirectX12Framework.h"

MaterialBuffer::MaterialBuffer() : m_materialBufferData(){}
MaterialBuffer::~MaterialBuffer(){}

void MaterialBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_materialBufferData = *data;
	memcpy(m_bufferGPUAddress[frame], &m_materialBufferData, sizeof(Data));
}
