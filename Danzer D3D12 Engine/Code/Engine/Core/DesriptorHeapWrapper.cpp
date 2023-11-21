#include "stdafx.h"
#include "DesriptorHeapWrapper.h"

DescriptorHeapWrapper::DescriptorHeapWrapper() :
	m_desc({}),
	m_cpuHeapStart({}),
	m_gpuHeapStart({}),
	m_handleIncrementSize(0)
{
}

void DescriptorHeapWrapper::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numberOfDescriptors, bool shaderVisible)
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

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeapWrapper::GET_CPU_DESCRIPTOR(const UINT offset)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_desctiptorHeap->GetCPUDescriptorHandleForHeapStart());
	if (offset != 0)
		cpuHandle.ptr += m_handleIncrementSize * offset;

	return cpuHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeapWrapper::GET_GPU_DESCRIPTOR(const UINT offset)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_desctiptorHeap->GetGPUDescriptorHandleForHeapStart());
	if (offset != 0)
		gpuHandle.ptr += m_handleIncrementSize * offset;

	return gpuHandle;
}
