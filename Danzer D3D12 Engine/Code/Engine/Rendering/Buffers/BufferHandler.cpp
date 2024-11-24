#include "stdafx.h"
#include "BufferHandler.h"

BufferHandler::BufferHandler(D3D12Framework& framework) : 
	m_framework(framework)
{
	m_bufferDatas.reserve(m_maxNumberOfBuffers);
}

ConstantBufferData* BufferHandler::CreateBufferData(const uint32_t sizeOfData)
{
	ConstantBufferData& data = m_bufferDatas.emplace_back();
	data.IntializeBuffer(m_framework.GetDevice(), &m_framework.CbvSrvHeap(), sizeOfData);

	return &data;
}

void BufferHandler::UpdateBufferDataForRendering()
{
	for (uint32_t i = 0; i < m_bufferDatas.size(); i++)
	{
		m_bufferDatas[i].UpdateBufferToGPU(m_framework.GetFrameIndex());
	}
}
