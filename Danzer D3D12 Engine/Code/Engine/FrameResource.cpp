#include "stdafx.h"
#include "FrameResource.h"

#include "DirectX-Headers-main/include/directx/d3d12.h"

FrameResource::FrameResource(ID3D12Device* device, const UINT index) :
	m_cmdIndex(index)
{
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList));
	m_cmdList->Close(); // Close CommandList, Don't want to record into it now

	m_cmdList->SetName(L"CmdList " + index);
}

void FrameResource::Initiate(ID3D12PipelineState* pso)
{
	CHECK_HR(m_cmdAllocator->Reset());
	CHECK_HR(m_cmdList->Reset(m_cmdAllocator.Get(), pso));
}

