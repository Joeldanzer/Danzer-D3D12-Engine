#pragma once
#include "../../Core/D3D12Header.h"

#include "DirectX/include/directx/d3dx12.h"

struct ID3D12Device;
class DescriptorHeapWrapper;

class ConstantBufferData
{
public:
#pragma warning ( suppress : 26495)
	ConstantBufferData() : m_offsetID(UINT32_MAX), m_sizeOfData(0)
	{}

	void UpdateBufferData(uint16_t* data);
	void IntializeBuffer(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper, const UINT sizeOfData);
	void UpdateBufferToGPU(const UINT frameIndex);

	uint32_t OffsetID() const {
		return m_offsetID;
	}
	
private:
	friend class BufferHandler;
	
	UINT AssignBufferSize(const UINT sizeOfData);

	ComPtr<ID3D12Resource> m_bufferUpload[FrameCount];
	uint16_t*			   m_bufferGPUAddress[FrameCount];
	uint16_t*			   m_newBufferData = nullptr;
	uint32_t			   m_offsetID = 0;

    uint16_t m_sizeOfData;
};

