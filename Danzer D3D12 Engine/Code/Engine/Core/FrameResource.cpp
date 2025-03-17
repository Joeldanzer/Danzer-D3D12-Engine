#include "stdafx.h"
#include "FrameResource.h"

#include "DirectX/include/directx/d3d12.h"

#include <string>

FrameResource::FrameResource(ID3D12Device* device, const uint32_t index, std::wstring cmdListName, const D3D12_COMMAND_LIST_TYPE type, const bool close) :
	m_cmdIndex(index)
{
	device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_cmdAllocator));
	device->CreateCommandList(0, type, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList));
	
	if (close) {
		m_cmdListIsOpen = false;
		m_cmdList->Close(); // Close CommandList, Don't want to record into it now
	}else
		m_cmdListIsOpen = true;


	cmdListName += std::to_wstring(index);
	m_cmdList->SetName(cmdListName.c_str());
}

FrameResource::~FrameResource()
{
	m_cmdAllocator.ReleaseAndGetAddressOf();
	m_cmdList.ReleaseAndGetAddressOf();
}

ID3D12GraphicsCommandList* FrameResource::Initiate(ID3D12PipelineState* pso)
{
	if (!m_cmdListIsOpen) {
		CHECK_HR(m_cmdAllocator->Reset());
		CHECK_HR(m_cmdList->Reset(m_cmdAllocator.Get(), pso));
		m_cmdListIsOpen = true;
	}

	return m_cmdList.Get();
}

void FrameResource::Close()
{
	if (m_cmdListIsOpen) {
		CHECK_HR(m_cmdList->Close());
		m_cmdListIsOpen = false;
	}
}

