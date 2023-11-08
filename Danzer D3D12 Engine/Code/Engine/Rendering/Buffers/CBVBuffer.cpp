#include "stdafx.h"
#include "CBVBuffer.h"

#include "../../Core/DirectX12Framework.h"

void CBVBuffer::Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, void* data, UINT sizeOfData)
{
	HRESULT result;
	
	UINT size = (sizeOfData + 255) & ~255;
	m_offsetID = 0;

	for (unsigned int i = 0; i < FrameCount; i++)
	{
		CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC   buffer     = CD3DX12_RESOURCE_DESC::Buffer(size);
		result = device->CreateCommittedResource(
			&uploadHeap,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_bufferUpload[i])
		);
		CHECK_HR(result);

		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(cbvWrapper->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

		cbvHandle.Offset(cbvWrapper->m_handleCurrentOffset * cbvWrapper->DESCRIPTOR_SIZE());

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_bufferUpload[i]->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = size; // Contant buffer size if required to be 256-byte aligned.
		device->CreateConstantBufferView(&cbvDesc, cbvHandle);

		m_offsetID = m_offsetID == 0 ? cbvWrapper->m_handleCurrentOffset : m_offsetID;
		cbvWrapper->m_handleCurrentOffset++;

		ZeroMemory(&data, sizeOfData);
		CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
		result = m_bufferUpload[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[i]));
		CHECK_HR(result);
		memcpy(m_bufferGPUAddress[i], &data, sizeOfData);
	}
}
