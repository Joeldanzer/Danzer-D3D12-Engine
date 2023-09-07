#include "stdafx.h"
#include "WindowSizeBuffer.h"

#include "../../Core/DirectX12Framework.h"

WindowBuffer::WindowBuffer(){}
WindowBuffer::~WindowBuffer()
{
	for (UINT i = 0; i < FrameCount; i++)
	{
		m_bufferUpload[i]->Unmap(0, 0);
		//m_mainDescriptorHeap[i].~ComPtr();
		m_bufferGPUAddress[i] = nullptr;
	}
}

void WindowBuffer::Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper)
{
	HRESULT result;

	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// Buffer size 256-byte alligned
		UINT size = (sizeof(WindowBuffer::Data) + 255) & ~255;
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
		cbvDesc.SizeInBytes = (sizeof(WindowBuffer::Data) + 255) & ~255; // Contant buffer size if required to be 256-byte aligned.
		device->CreateConstantBufferView(&cbvDesc, cbvHandle);

		m_offsetID = cbvWrapper->m_handleCurrentOffset;
		cbvHandle.Offset(cbvWrapper->DESCRIPTOR_SIZE());
		cbvWrapper->m_handleCurrentOffset += cbvWrapper->DESCRIPTOR_SIZE();

		ZeroMemory(&m_windowData, sizeof(WindowBuffer::Data));
		CD3DX12_RANGE readRange(0, 0); // Don't intend to read this resource on the CPU
		result = m_bufferUpload[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferGPUAddress[i]));
		CHECK_HR(result);
		memcpy(m_bufferGPUAddress[i], &m_windowData, sizeof(WindowBuffer::Data));
	}
}

void WindowBuffer::UpdateBuffer(UINT frameIndex, void* cbvData)
{
	WindowBuffer::Data* data = reinterpret_cast<WindowBuffer::Data*>(cbvData);
	m_windowData = *data;
	memcpy(m_bufferGPUAddress[frameIndex], &m_windowData, sizeof(WindowBuffer::Data));
}
