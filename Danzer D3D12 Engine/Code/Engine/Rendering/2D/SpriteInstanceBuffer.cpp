#include "stdafx.h"
#include "SpriteInstanceBuffer.h"

#include "SpriteData.h"

SpriteInstanceBuffer::SpriteInstanceBuffer(){}
SpriteInstanceBuffer::~SpriteInstanceBuffer(){
}

void SpriteInstanceBuffer::Init(ID3D12Device* device)
{

	UINT size = sizeof(SpriteData::Instance) * MAX_INSTANCES_PER_MODEL;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CreateHeap(device, &m_vertexBuffer[i], D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
		m_vertexBufferView[i].BufferLocation = m_vertexBuffer[i]->GetGPUVirtualAddress();
		m_vertexBufferView[i].SizeInBytes = sizeof(SpriteData::Instance) * MAX_INSTANCES_PER_MODEL;
		m_vertexBufferView[i].StrideInBytes = sizeof(SpriteData::Instance);
	}
	
}

void SpriteInstanceBuffer::UpdateBuffer(UINT8* instances, UINT size, UINT frameIndex)
{
	CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
	HRESULT result = m_vertexBuffer[frameIndex]->Map(0, &readRange, reinterpret_cast<void**>(&m_data[frameIndex]));
	CHECK_HR(result);
	memcpy(m_data[frameIndex], instances, sizeof(SpriteData::Instance) * size); //ptrData;
	m_vertexBuffer[frameIndex]->Unmap(0, nullptr);
}

