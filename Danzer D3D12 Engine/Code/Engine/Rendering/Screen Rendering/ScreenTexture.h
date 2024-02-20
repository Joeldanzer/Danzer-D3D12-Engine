#pragma once
#include "Core/D3D12Header.h"
#include "DirectX-Headers-main\include\directx\d3dx12.h"

class D3D12Framework;

struct ID3D12DescriptorHeap;
struct ID3D12Resource;

class ScreenTexture
{
public:
	ScreenTexture(D3D12Framework& framework, DXGI_FORMAT format, UINT width, UINT height, std::wstring name);
	~ScreenTexture();

	ID3D12Resource* GetResource() {
		return m_resource.Get();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCPUDescHandle() {
		m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart();
	}
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGPUDescHandle() {
		m_rtvDescriptor->GetGPUDescriptorHandleForHeapStart();
	}

private:

	ComPtr<ID3D12DescriptorHeap> m_rtvDescriptor;
	ComPtr<ID3D12DescriptorHeap> m_srvDescriptor;

	ComPtr<ID3D12Resource> m_resource;
};

