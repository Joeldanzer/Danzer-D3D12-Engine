#include "stdafx.h"
#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{	
}

void VertexBuffer::Initialize(ID3D12Device* device, uint32_t sizeOfData, uint32_t maxInstances)
{
	m_sizeOfData   = sizeOfData;
	m_maxInstances = maxInstances;
	uint32_t size  = sizeOfData * maxInstances;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CreateHeap(device, &m_vertexBuffer[i], D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
		m_vertexBufferView[i].BufferLocation = m_vertexBuffer[i]->GetGPUVirtualAddress();
		m_vertexBufferView[i].SizeInBytes    = size;
		m_vertexBufferView[i].StrideInBytes  = sizeOfData;
	}
}

void VertexBuffer::UpdateBuffer(uint16_t* bufferData, const uint32_t instanceCount, const uint8_t frameIndex)
{
	if (instanceCount <= 0)
		return;

	CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
	CHECK_HR(m_vertexBuffer[frameIndex]->Map(0, &readRange, reinterpret_cast<void**>(&m_data[frameIndex])));
	
	// Only send in the required amount of data to the vertex buffer. 
	const uint32_t count = instanceCount > m_maxInstances ? m_maxInstances : instanceCount;
	memcpy(m_data[frameIndex], bufferData, count * m_sizeOfData);
	m_vertexBuffer[frameIndex]->Unmap(0, nullptr);
}

void VertexBuffer::CreateHeap(ID3D12Device* device, ID3D12Resource** resource, D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES resourceState, UINT size)
{
	CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(type);
	CD3DX12_RESOURCE_DESC buffer       = CD3DX12_RESOURCE_DESC::Buffer(size);

	CHECK_HR(
		device->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		resourceState,
		nullptr,
		IID_PPV_ARGS(resource)
		));
}


