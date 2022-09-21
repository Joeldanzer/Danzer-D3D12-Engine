#pragma once

#include "../../Core/D3D12Header.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"
#include <vector>


class DirectX12Framework;

class VertexBuffer
{
public:
	VertexBuffer() : m_vertexBufferView(), m_data(), m_vertexBuffer(){}
	virtual ~VertexBuffer();

	virtual void Init(ID3D12Device* device) { device; }
	virtual void UpdateBuffer(UINT8* bufferData, UINT size, UINT frameIndex) { bufferData; size; frameIndex;}

	D3D12_VERTEX_BUFFER_VIEW& GetBufferView(UINT frameIndex) { return m_vertexBufferView[frameIndex]; }

protected:
	void CreateHeap(ID3D12Device* device, ID3D12Resource** heap, D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES resourceState, UINT size);

	UINT8* m_data[FrameCount];
	ID3D12Resource* m_vertexBuffer[FrameCount];
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView[FrameCount];
};

