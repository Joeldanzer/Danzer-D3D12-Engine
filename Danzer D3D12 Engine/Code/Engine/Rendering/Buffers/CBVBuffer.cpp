#include "stdafx.h"
#include "CBVBuffer.h"

#include "../../Core/D3D12Framework.h"

CBVBuffer::~CBVBuffer()
{
}

void CBVBuffer::Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, UINT sizeOfData)
{	
	m_sizeOfData = sizeOfData;
	UINT size = AssignBufferSize(sizeOfData);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle;
	m_offsetID = cbvWrapper->CreateDescriptorHandle(cbvHandle, 3);

	const UINT actualSize = sizeOfData;
	for (unsigned int i = 0; i < FrameCount; i++)
	{
		CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC   buffer     = CD3DX12_RESOURCE_DESC::Buffer(size);
		
		CHECK_HR(device->CreateCommittedResource(
			&uploadHeap,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_bufferUpload[i])
		));

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_bufferUpload[i]->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = size; // Contant buffer size is required to be 256-byte aligned.
		
		device->CreateConstantBufferView(&cbvDesc, cbvHandle);
		cbvHandle.Offset(cbvWrapper->DESCRIPTOR_SIZE());

		CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
		CHECK_HR(m_bufferUpload[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[i])));
	}
}

void CBVBuffer::UpdateBuffer(UINT16* cbvData, const UINT frame)
{
	memcpy(m_bufferGPUAddress[frame], cbvData, m_sizeOfData);
}

void CBVBuffer::Release()
{
	for (UINT i = 0; i < FrameCount; i++)
	{
		m_bufferUpload[i]->Unmap(0, 0);
		m_bufferUpload[i]->Release();

		delete m_bufferGPUAddress[i];
	}
}

UINT CBVBuffer::AssignBufferSize(const UINT sizeOfData)
{
	UINT size = 0;
	if (sizeOfData > 256 && sizeOfData < 512)
		size = (sizeOfData + 511)  & ~511;
	else if (sizeOfData > 512 && sizeOfData < 1024)
		size = (sizeOfData + 1023) & ~1023;
	else
		size = (sizeOfData + 255)  & ~255;

	return size;
}
