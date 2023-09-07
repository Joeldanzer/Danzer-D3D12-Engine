#include "stdafx.h"
#include "CameraBuffer.h"

#include "../../Core/DirectX12Framework.h"

CameraBuffer::CameraBuffer()
{
}

CameraBuffer::~CameraBuffer()
{
}

void CameraBuffer::Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper)
{
	HRESULT result;
	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		UINT size = (sizeof(CameraBuffer::Data) + 255) & ~255;
		CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(size);
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

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_bufferUpload[i]->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (sizeof(CameraBuffer::Data) + 255) & ~255; // Contant buffer size if required to be 256-byte aligned.
		device->CreateConstantBufferView(&cbvDesc, cbvHandle);

		cbvHandle.Offset(cbvWrapper->DESCRIPTOR_SIZE());
		cbvWrapper->m_handleCurrentOffset += cbvWrapper->DESCRIPTOR_SIZE();

		ZeroMemory(&m_cameraBufferData, sizeof(CameraBuffer::Data));
		CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
		result = m_bufferUpload[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[i]));
		CHECK_HR(result);
		memcpy(m_bufferGPUAddress[i], &m_cameraBufferData, sizeof(CameraBuffer::Data));
	}
}

void CameraBuffer::UpdateBuffer(/*ID3D12GraphicsCommandList* cmdList,*/ UINT frameIndex, void* cbvData)
{
	Data* data = reinterpret_cast<Data*>(cbvData);
	m_cameraBufferData = *data;
	memcpy(m_bufferGPUAddress[frameIndex], &m_cameraBufferData, sizeof(Data));
}
