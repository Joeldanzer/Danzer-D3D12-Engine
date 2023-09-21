#include "stdafx.h"
#include "CameraBuffer.h"

#include "../../Core/DirectX12Framework.h"

CameraBuffer::CameraBuffer()
{
}

CameraBuffer::~CameraBuffer()
{
}

void CameraBuffer::UpdateBuffer(void* cbvData)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_cameraBufferData = *data;
	memcpy(m_bufferGPUAddress, &m_cameraBufferData, sizeof(Data));
}
