#pragma once
#include "Core/D3D12Header.h"
#include "Core/MathDefinitions.h"

#include "DirectX-Headers-main\include\directx\d3dx12.h"

#include <array>
#include <vector>

class D3D12Framework;
class DescriptorHeapWrapper;
class PSOHandler;

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
	GBuffer(D3D12Framework& framework, PSOHandler& psoHandler);
	~GBuffer();

	static std::array<DXGI_FORMAT, GBUFFER_COUNT> GBufferFormats(){
		std::array<DXGI_FORMAT, GBUFFER_COUNT> format = {
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* ALBEDO
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* NORMAL
		DXGI_FORMAT_R8G8B8A8_UNORM,		 //* MATERIAL
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* VERTEX COLOR
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* VERTEX NORMAL
		DXGI_FORMAT_R32G32B32A32_FLOAT,  //* WORLD POSITION
		DXGI_FORMAT_R32_FLOAT			 //* DEPTH TEXTURE
		};
		return format;
	}

	std::array<ID3D12Resource*, GBUFFER_COUNT> GetGbufferResources();
	std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, GBUFFER_COUNT> GetRTVDescriptorHandles(DescriptorHeapWrapper& rtvDesc);

	void ClearRenderTargets(DescriptorHeapWrapper& rtvHeap, ID3D12GraphicsCommandList* cmdList, UINT numberOfRects, const D3D12_RECT* rect);
	void AssignSRVSlots(ID3D12GraphicsCommandList* cmdList,   DescriptorHeapWrapper* srvWrapper, UINT& startLocation);
	void SetTextureAtSlot(ID3D12GraphicsCommandList* cmdList, GBUFFER_TEXTURES texture, DescriptorHeapWrapper* srvWrapper, const UINT slot);

	void SetPSOAndRS(const UINT pso, const UINT rs) {
		m_pso = pso; m_rs = rs;
	}

	const UINT GetTransparentPSO() { return m_transparentPso;}
	const UINT GetPSO()			   { return m_pso; }
	const UINT GetRootSignature()  { return m_rs; }

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(GBUFFER_TEXTURES texture, DescriptorHeapWrapper* srvWrapper);

	struct Resource {
		UINT m_offsetID = 0;
		ComPtr<ID3D12Resource> m_resource;
	};
	

private:
	void InitializeGBuffers(D3D12Framework& framework, PSOHandler& psoHandler);
	void InitPipelineAndRootSignature(PSOHandler& psoHandler);

	UINT m_rtvDescSize;
	UINT m_srvDescSize;

	UINT m_pso;
	UINT m_transparentPso;
	UINT m_rs;

	UINT m_rtvOffsetID;

	std::array<UINT, GBUFFER_COUNT>        m_rtvHeapSize; 
	std::array<Resource, GBUFFER_COUNT>    m_resources;
	std::array<DXGI_FORMAT, GBUFFER_COUNT> m_formats;
	

	//ComPtr<ID3D12DescriptorHeap>        m_rtvDescriptor;
};

