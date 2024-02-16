#include "stdafx.h"
#include "PointLightBuffer.h"

#include "../Engine/Core/D3D12Header.h"
#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

void PointLightBuffer::UpdateBuffer(void* cbvData, unsigned int frame)
{
	//m_bufferUpload[frame]->Unmap()
	//ZeroMemory(&m_bufferGPUAddress[frame], sizeof(Data));
	//CHECK_HR(m_bufferUpload[frame]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[frame])));

	ZeroMemory(&m_lightBufferData, sizeof(Data));

	CD3DX12_RANGE readRange(0, 0);
	CHECK_HR(m_bufferUpload[frame]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[frame])));
	m_lightBufferData = *reinterpret_cast<Data*>(cbvData);
	memcpy(m_bufferGPUAddress[frame], &m_lightBufferData, sizeof(Data));
	m_bufferUpload[frame]->Unmap(0, &readRange);
	//CD3DX12_RANGE readRange(0, 0); 
	//m_bufferUpload[frame]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[frame]));
	//
	//m_bufferUpload[frame]->Unmap(, &readRange);
}
