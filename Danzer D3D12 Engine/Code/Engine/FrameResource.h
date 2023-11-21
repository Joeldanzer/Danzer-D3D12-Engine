#pragma once
#include "Core/D3D12Header.h"

struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandAllocator;
struct ID3D12Device;

class FrameResource{
public:
	FrameResource(ID3D12Device* device, const UINT index);
	
	ID3D12GraphicsCommandList* CmdList() {
		return m_cmdList.Get();
	}	

	void Initiate(ID3D12PipelineState* = nullptr);

	const UINT CmdListIndex() {
		return m_cmdIndex;
	}

private:
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12CommandAllocator>    m_cmdAllocator;

	const UINT m_cmdIndex;
};

