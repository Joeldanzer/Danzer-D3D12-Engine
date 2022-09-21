#include "stdafx.h"
#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{
	for (UINT i = 0; i < FrameCount; i++)
	{
		m_vertexBuffer[i] = nullptr;
	}
}

void VertexBuffer::CreateHeap(ID3D12Device* device, ID3D12Resource** resource, D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES resourceState, UINT size)
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(type);
	CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(size);

	result = device->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		resourceState,
		nullptr,
		IID_PPV_ARGS(resource)
	);
	CHECK_HR(result);
}


