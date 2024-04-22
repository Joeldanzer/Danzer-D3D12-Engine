#include "stdafx.h"
#include "ConstantBufferData.h"

#include "../../Core/D3D12Framework.h"

void ConstantBufferData::IntializeBuffer(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, const UINT sizeOfData)
{
	UINT size    = AssignBufferSize(sizeOfData);
	m_sizeOfData = sizeOfData;

	m_offsetID = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(cbvWrapper->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(cbvWrapper->m_handleCurrentOffset * cbvWrapper->DESCRIPTOR_SIZE());

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

		m_offsetID = m_offsetID == 0 ? cbvWrapper->m_handleCurrentOffset : m_offsetID;
		cbvHandle.Offset(cbvWrapper->DESCRIPTOR_SIZE());
		cbvWrapper->m_handleCurrentOffset++;

		CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
		CHECK_HR(m_bufferUpload[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[i])));
	}
}

void ConstantBufferData::UpdateBuffer(UINT16* data, const UINT frameIndex)
{
	memcpy(m_bufferGPUAddress[frameIndex], data, m_sizeOfData);
}

UINT ConstantBufferData::AssignBufferSize(const UINT sizeOfData)
{
	UINT size = 0;
	if (sizeOfData > 256 && sizeOfData < 512)
		size = (sizeOfData + 511) & ~511;
	else if (sizeOfData > 512 && sizeOfData < 1024)
		size = (sizeOfData + 1023) & ~1023;
	else
		size = (sizeOfData + 255) & ~255;

	return size;
}

