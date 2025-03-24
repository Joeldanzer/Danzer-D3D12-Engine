#include "stdafx.h"
#include "DesriptorHeapWrapper.h"

DescriptorHeapWrapper::DescriptorHeapWrapper() :
	m_desc({}),
	m_cpuHeapStart({}),
	m_gpuHeapStart({}),
	m_handleIncrementSize(0)
{
}

DescriptorHeapWrapper::~DescriptorHeapWrapper()
{
	m_gpuHeapStart.ptr = 0;
	m_cpuHeapStart.ptr = 0;
	m_desctiptorHeap.~ComPtr();
}

void DescriptorHeapWrapper::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numberOfDescriptors, bool shaderVisible)
{
	m_desc = { };
	m_desc.Type = type;
	m_desc.NumDescriptors = numberOfDescriptors;
	m_desc.Flags = (shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	CHECK_HR(device->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&m_desctiptorHeap)));

	m_cpuHeapStart = m_desctiptorHeap->GetCPUDescriptorHandleForHeapStart();
	if(shaderVisible) m_gpuHeapStart = m_desctiptorHeap->GetGPUDescriptorHandleForHeapStart();

	m_handleIncrementSize = device->GetDescriptorHandleIncrementSize(type);
}


CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeapWrapper::GET_CPU_DESCRIPTOR(const uint32_t offset)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	cpuHandle.InitOffsetted(m_desctiptorHeap->GetCPUDescriptorHandleForHeapStart(), offset * m_handleIncrementSize);

	return cpuHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeapWrapper::GET_GPU_DESCRIPTOR(const uint32_t offset)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	gpuHandle.InitOffsetted(m_desctiptorHeap->GetGPUDescriptorHandleForHeapStart(), m_handleIncrementSize * offset);
	return gpuHandle;
}

uint32_t DescriptorHeapWrapper::CreateDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const uint32_t count )
{
	while(m_waitForHeapCreation){}
	m_waitForHeapCreation = true;
	assert(m_handleCurrentOffset < m_desc.NumDescriptors);

	cpuHandle = GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.Offset((m_handleCurrentOffset * DESCRIPTOR_SIZE()));

	const uint32_t offset = m_handleCurrentOffset;
	m_handleCurrentOffset+=count;
	
	m_waitForHeapCreation = false;
	return offset;
}

uint32_t DescriptorHeapWrapper::CreateDescriptorHandle(const uint32_t count)
{
	while (m_waitForHeapCreation) {}
	m_waitForHeapCreation = true;
	assert(m_handleCurrentOffset < m_desc.NumDescriptors);

	const uint32_t offset = m_handleCurrentOffset;
	m_handleCurrentOffset += count;

	m_waitForHeapCreation = false;
	return offset;
}
