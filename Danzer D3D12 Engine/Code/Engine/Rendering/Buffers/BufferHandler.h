#pragma once
#include "ConstantBufferData.h"

class D3D12Framework;

class BufferHandler
{
public:
	BufferHandler(D3D12Framework& framework);

	ConstantBufferData* CreateBufferData(const uint32_t sizeOfData);

private:
	const uint32 m_maxNumberOfBuffers = USHRT_MAX;
	friend class RenderManager;
	
	void UpdateBufferDataForRendering(); // Sets the updated date for the constant buffers GPU address

	std::vector<ConstantBufferData> m_bufferDatas;
	D3D12Framework& m_framework;
};


