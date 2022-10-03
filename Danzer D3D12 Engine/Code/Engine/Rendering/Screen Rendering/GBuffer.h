#pragma once
#include "Core/D3D12Header.h"
#include "Core/MathDefinitions.h"

#include "DirectX-Headers-main\include\directx\d3dx12.h"

#include <array>
#include <vector>

class DirectX12Framework;

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
	//Add more when necessary
	GBUFFER_COUNT
};

class GBuffer
{
public:
	GBuffer(DirectX12Framework& framework);
	~GBuffer();

	//std::vector<ID3D12Resource*> GetGbufferResources(UINT frameCount);
	std::array<ID3D12Resource*, GBUFFER_COUNT> GetGbufferResources();

	std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> GetRTVDescriptorHandles();
	//CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVDescriptorHandles();

	void ClearRenderTargets(ID3D12GraphicsCommandList* cmdList, Vect4f clearColor, UINT numberOfRects, const D3D12_RECT* rect);
	void AssignSRVSlots(ID3D12GraphicsCommandList* cmdList, UINT startLocation);

	UINT GetRTVDescSize() {
		return m_rtvDescSize;
	}
	UINT GetSRVDescSize() {
		return m_srvDescSize;
	}

private:
	void InitializeGBuffers(DirectX12Framework& framework);

	UINT m_rtvDescSize;
	UINT m_srvDescSize;

	//std::array<UINT, GBUFFER_COUNT> m_rtvHeapSize; 
	//std::array<ComPtr<ID3D12DescriptorHeap>, GBUFFER_COUNT> m_rtvDescriptor;
	ComPtr<ID3D12DescriptorHeap> m_rtvDescriptor;
	ComPtr<ID3D12DescriptorHeap> m_srvDescriptor;

	//std::array<ComPtr<ID3D12DescriptorHeap>, GBUFFER_COUNT> m_srvDescriptor;
	//std::array<std::array<ComPtr<ID3D12Resource>, FrameCount>, GBUFFER_COUNT> m_resources2;
	std::array<ComPtr<ID3D12Resource>, GBUFFER_COUNT> m_resources;
};

