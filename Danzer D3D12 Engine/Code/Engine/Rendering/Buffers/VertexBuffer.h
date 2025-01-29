#pragma once

#include "Core/D3D12Header.h"
#include "DirectX/include/directx/d3dx12.h"

#define MAX_INSTANCES_PER_VERTEX_BUFFER 10000u

class VertexBuffer
{
public:
	VertexBuffer() : m_sizeOfData(0), m_maxInstances(0), m_vertexBufferView(), m_data(), m_vertexBuffer() {}
	~VertexBuffer();

	void Initialize(ID3D12Device* device, uint32_t sizeOfData, uint32_t maxInstances = MAX_INSTANCES_PER_VERTEX_BUFFER);
	void UpdateBuffer(uint16_t* bufferData, const uint32_t instanceCount, const uint8_t frameIndex);

	D3D12_VERTEX_BUFFER_VIEW& GetBufferView(UINT frameIndex) { return m_vertexBufferView[frameIndex]; }

protected:
	void CreateHeap(ID3D12Device* device, ID3D12Resource** heap, D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES resourceState, UINT size);
		
	uint32_t				 m_sizeOfData;
	uint32_t				 m_maxInstances;

	uint16_t*				 m_data[FrameCount];
	ID3D12Resource*			 m_vertexBuffer[FrameCount];
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView[FrameCount];
};

