#pragma once
#include "DirectX/include/directx/d3dx12.h"
#include "D3D12Header.h"

#define MAX_NUMBER_OF_DESCTRIPTORS 1000000
#define MAX_NUMBER_OF_RTV 200

class DescriptorHeapWrapper
{
public:
	DescriptorHeapWrapper();
	~DescriptorHeapWrapper();

	void CreateDescriptorHeap(ID3D12Device* device,
							  D3D12_DESCRIPTOR_HEAP_TYPE type,
							  UINT numberOfDescriptors,
							  bool shaderVisible);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GET_CPU_DESCRIPTOR(const UINT size); 
	CD3DX12_GPU_DESCRIPTOR_HANDLE GET_GPU_DESCRIPTOR(const UINT size); 
    
	ID3D12DescriptorHeap*        GetDescriptorHeap()	  { return m_desctiptorHeap.Get(); }
	const UINT					 DESCRIPTOR_SIZE()        { return m_handleIncrementSize; }
	
	UINT						 m_handleCurrentOffset; //CurrentOffset * IncrementSize

private:
	D3D12_DESCRIPTOR_HEAP_DESC m_desc;
	ComPtr<ID3D12DescriptorHeap> m_desctiptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHeapStart; 

	UINT m_handleIncrementSize; 
};

