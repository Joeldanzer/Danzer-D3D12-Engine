#pragma once
#include "Core/D3D12Header.h"

struct ID3D12CommandQueue;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandAllocator;
struct ID3D12Device;

class FrameResource{
public:
	FrameResource(ID3D12Device* device, const uint32_t index, std::wstring cmdListName, const D3D12_COMMAND_LIST_TYPE type, const bool close = true);
	~FrameResource();

	ID3D12GraphicsCommandList* CmdList() {
		return m_cmdList.Get();
	}	
	bool CmdListIsOpen() {
		return m_cmdListIsOpen;
	}
	const uint32_t CmdListIndex() {
		return m_cmdIndex;
	}

	ID3D12GraphicsCommandList* Initiate(ID3D12PipelineState* = nullptr);

private:
	friend class D3D12Framework;
	
	void Close();

	bool							  m_cmdListIsOpen;
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12CommandAllocator>    m_cmdAllocator;

	const uint32_t m_cmdIndex;
};

