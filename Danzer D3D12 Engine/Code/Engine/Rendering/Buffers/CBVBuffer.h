#pragma once

#include "../../Core/D3D12Header.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

struct ID3D12Device;
struct DescriptorHeapWrapper;

// Class that holds all the needed infromation for any kind of CBV buffer
class CBVBuffer
{
public:
	CBVBuffer() : m_bufferGPUAddress(){}
	virtual ~CBVBuffer(){ 
		for (UINT i = 0; i < FrameCount; i++)
		{
			//m_bufferUpload[i]->Unmap(0, 0);
			//m_bufferUpload[i].~ComPtr();
			//m_bufferGPUAddress[i] = nullptr;
			//m_mainDescriptorHeap[i].~ComPtr();
		}
	}
	
	void Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, void* data, UINT sizeOfData);
	// Takes current frameIndex as well as Specific buffer Data(Check CameraBuffer for example)
	virtual void UpdateBuffer(void* cbvData, unsigned int frame) = 0 { cbvData; frame; }

	const UINT OffsetID() { return m_offsetID; }
protected:
	UINT AssignBufferSize(const UINT sizeOfData);
	// Need buffer information for each back buffer available. 
	ComPtr<ID3D12Resource>		  m_bufferUpload[FrameCount];
	UINT16*						  m_bufferGPUAddress[FrameCount];
	UINT						  m_offsetID = 0; 
};
