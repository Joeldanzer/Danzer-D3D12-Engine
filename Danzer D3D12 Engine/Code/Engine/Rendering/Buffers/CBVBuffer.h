#pragma once

#include "Core/D3D12Header.h"
#include "DirectX/include/directx/d3dx12.h"

struct ID3D12Device;
class  DescriptorHeapWrapper;


// Class that holds all the needed infromation for any kind of CBV buffer
class CBVBuffer
{
public:
	CBVBuffer() : m_bufferGPUAddress(){}
	~CBVBuffer();
	
	void Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, UINT sizeOfData);

	// Takes current frameIndex as well as Specific buffer Data(Check CameraBuffer for example)
	void UpdateBuffer(UINT16* cbvData, const UINT frame); //= 0 { cbvData; frame; }

	void Release();

	const UINT OffsetID() { return m_offsetID; }
protected:
	UINT AssignBufferSize(const UINT sizeOfData);

	// Need buffer information for each back buffer available. 
	ComPtr<ID3D12Resource>		  m_bufferUpload[FrameCount];
	UINT16*						  m_bufferGPUAddress[FrameCount];
	UINT						  m_offsetID = UINT32_MAX; 
	UINT						  m_sizeOfData = 0;

};
