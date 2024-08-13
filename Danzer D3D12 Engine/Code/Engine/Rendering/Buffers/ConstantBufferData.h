#pragma once
#include "../../Core/D3D12Header.h"

#include "DirectX/include/directx/d3dx12.h"

struct ID3D12Device;
class DescriptorHeapWrapper;

class ConstantBufferData
{
public:
#pragma warning ( suppress : 26495)
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

