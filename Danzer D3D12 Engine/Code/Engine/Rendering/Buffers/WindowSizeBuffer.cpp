#include "stdafx.h"
#include "WindowSizeBuffer.h"

#include "../../Core/DirectX12Framework.h"

WindowBuffer::WindowBuffer(){}
WindowBuffer::~WindowBuffer()
{
}


void WindowBuffer::UpdateBuffer(void* cbvData)
{
	WindowBuffer::Data* data = reinterpret_cast<WindowBuffer::Data*>(cbvData);
	m_windowData = *data;
	memcpy(m_bufferGPUAddress, &m_windowData, sizeof(WindowBuffer::Data));
}
