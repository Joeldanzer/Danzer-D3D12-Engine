#pragma once
#include "../../Core/D3D12Header.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

struct ID3D12Device;
struct DescriptorHeapWrapper;

class ConstantBufferData
{
public:
	ConstantBufferData() : m_offsetID(0), m_sizeOfData(0){}
	void IntializeBuffer(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, const UINT sizeOfData);

	void UpdateBuffer(UINT16* data,  const UINT frameIndex);

	const UINT OffsetID() {
		return m_offsetID;
	}

private:
	UINT AssignBufferSize(const UINT sizeOfData);

	ComPtr<ID3D12Resource> m_bufferUpload[FrameCount];
	UINT16*				   m_bufferGPUAddress[FrameCount];
	UINT				   m_offsetID = 0;

    UINT m_sizeOfData;
};

