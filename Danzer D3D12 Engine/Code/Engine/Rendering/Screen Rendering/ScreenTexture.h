#pragma once
#include "Core/D3D12Header.h"
#include "DirectX-Headers-main\include\directx\d3dx12.h"

class DirectX12Framework;

struct ID3D12DescriptorHeap;
struct ID3D12Resource;

class ScreenTexture
{
public:
	ScreenTexture(DirectX12Framework& framework, DXGI_FORMAT format, UINT width, UINT height, std::wstring name);
	~ScreenTexture();

	ID3D12Resource* GetResource() {
		return m_resource.Get();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVDescHandle() {
		m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart();
	}


private:

	ComPtr<ID3D12DescriptorHeap> m_rtvDescriptor;
	ComPtr<ID3D12DescriptorHeap> m_srvDescriptor;

	ComPtr<ID3D12Resource> m_resource;


};

