#include "stdafx.h"
#include "AABBBuffer.h"
#include "Components/Collision/AABBCollider.h"


void AABBBuffer::Init(ID3D12Device* device)
{
	UINT size = sizeof(AABBBuffer::AABBInstance) * MAX_INSTANCES_PER_MODEL;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CreateHeap(device, &m_vertexBuffer[i], D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
		m_vertexBufferView[i].BufferLocation = m_vertexBuffer[i]->GetGPUVirtualAddress();
		m_vertexBufferView[i].SizeInBytes = size;
		m_vertexBufferView[i].StrideInBytes = sizeof(AABBBuffer::AABBInstance);
	}
}

void AABBBuffer::UpdateBuffer(UINT8* aabb, UINT size, UINT frameIndex)
{
	CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
	HRESULT result = m_vertexBuffer[frameIndex]->Map(0, &readRange, reinterpret_cast<void**>(&m_data[frameIndex]));//ZeroMemory(m_data, sizeof(Mat4f) * MAX_INSTANCES_PER_MODEL);
	CHECK_HR(result);
	memcpy(m_data[frameIndex], aabb, size * sizeof(AABBBuffer::AABBInstance));
	m_vertexBuffer[frameIndex]->Unmap(0, nullptr);
}
