#pragma once
#include "Core/D3D12Header.h"
#include "Core/MathDefinitions.h"

#include "DirectX-Headers-main\include\directx\d3dx12.h"

#include <array>
#include <vector>

class D3D12Framework;
class DescriptorHeapWrapper;

struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12GraphicsCommandList;

enum GBUFFER_TEXTURES{
	GBUFFER_ALBEDO,
	GBUFFER_NORMAL,
	GBUFFER_MATERIAL,
	GBUFFER_VERTEX_COLOR,
	GBUFFER_VERTEX_NORMAL,
	GBUFFER_WORLD_POSITION,
	GBUFFER_DEPTH,
	//Add more when necessary
	GBUFFER_COUNT
};

class GBuffer
{
public:
	GBuffer(D3D12Framework& framework);
	~GBuffer();

	std::array<ID3D12Resource*, GBUFFER_COUNT> GetGbufferResources();
	std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> GetRTVDescriptorHandles(DescriptorHeapWrapper& rtvDesc);

	void ClearRenderTargets(DescriptorHeapWrapper& rtvHeap, ID3D12GraphicsCommandList* cmdList, UINT numberOfRects, const D3D12_RECT* rect);
	void AssignSRVSlots(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* srvWrapper, UINT& startLocation);

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(GBUFFER_TEXTURES texture, DescriptorHeapWrapper* srvWrapper);

	struct Resource {
		UINT m_offsetID = 0;
		ComPtr<ID3D12Resource> m_resource;
	};
	

private:
	void InitializeGBuffers(D3D12Framework& framework);

	UINT m_rtvDescSize;
	UINT m_srvDescSize;


	UINT m_rtvOffsetID;

	std::array<UINT, GBUFFER_COUNT>     m_rtvHeapSize; 
	std::array<Resource, GBUFFER_COUNT> m_resources;
	

	//ComPtr<ID3D12DescriptorHeap>        m_rtvDescriptor;
};

