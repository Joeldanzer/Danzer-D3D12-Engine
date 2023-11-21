#include "stdafx.h"
#include "WindowSizeBuffer.h"

#include "../../Core/D3D12Framework.h"

WindowBuffer::WindowBuffer(){}
WindowBuffer::~WindowBuffer()
{
}


void WindowBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	WindowBuffer::Data* data = reinterpret_cast<WindowBuffer::Data*>(cbvData);
	m_windowData = *data;
	memcpy(m_bufferGPUAddress[frame], &m_windowData, sizeof(WindowBuffer::Data));
}
