#pragma once

#include "../../Core/D3D12Header.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

struct ID3D12Device;

// Class that holds all the needed infromation for any kind of CBV buffer
class CBVBuffer
{
public:
	CBVBuffer() : m_bufferGPUAddress(){}
	virtual ~CBVBuffer(){ 
		for (UINT i = 0; i < FrameCount; i++)
		{
			//m_bufferUpload[i]->Unmap(0, 0);
			m_bufferUpload[i].~ComPtr();
			m_mainDescriptorHeap[i].~ComPtr();
			m_bufferGPUAddress[i] = nullptr;
		}
	}

	virtual void Init(ID3D12Device* device) { device; }

	// Takes current frameIndex as well as Specific buffer Data(Check CameraBuffer for example)
	virtual void UpdateBuffer(UINT frameIndex, void* cbvData) { frameIndex; cbvData; }
	
	ID3D12DescriptorHeap* GetDescriptorHeap(UINT frameIndex) { return m_mainDescriptorHeap[frameIndex].Get(); }

protected:
	// Need buffer information for each back buffer available. 
	ComPtr<ID3D12Resource>		  m_bufferUpload[FrameCount];
	ComPtr<ID3D12DescriptorHeap>  m_mainDescriptorHeap[FrameCount];
	UINT8* m_bufferGPUAddress[FrameCount];
};
