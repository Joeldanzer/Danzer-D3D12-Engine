#include "stdafx.h"
#include "CameraBuffer.h"

#include "../../Core/D3D12Framework.h"

CameraBuffer::CameraBuffer(){}
CameraBuffer::~CameraBuffer(){}

void CameraBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_cameraBufferData = *data;
	memcpy(m_bufferGPUAddress[frame], &m_cameraBufferData, sizeof(Data));
}
