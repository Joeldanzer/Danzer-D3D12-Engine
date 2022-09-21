#include "stdafx.h"
#include "ScreenTexture.h"

#include "Core\DirectX12Framework.h"


ScreenTexture::ScreenTexture(DirectX12Framework& framework, DXGI_FORMAT format, UINT width, UINT height, std::wstring name)
{
	HRESULT result;

	ID3D12Device* device = framework.GetDevice();
	IDXGISwapChain* swapChain = framework.GetSwapChain();

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
		rtvDesc.NumDescriptors = FrameCount;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		result = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvDescriptor));
		CHECK_HR(result);


		D3D12_RESOURCE_DESC srvDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			format, width, height,
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		float clearColor[4] = { 0.5f, 0.5f, 1.f, 1.f };
		D3D12_CLEAR_VALUE clearValue;
		memcpy(&clearValue.Color[0], &clearColor[0], sizeof(float) * 4);
		clearValue.Format = format;

		result = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&srvDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&clearValue,
			IID_PPV_ARGS(&m_rtvDescriptor));
		CHECK_HR(result);

		UINT rtvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		UINT srvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//m_rtvHeapSize[i] = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptor->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_srvDescriptor->GetCPUDescriptorHandleForHeapStart());

		device->CreateRenderTargetView(m_resource.Get(), nullptr, rtvHandle);
		device->CreateShaderResourceView(m_resource.Get(), nullptr, srvHandle);

		m_resource->SetName((LPCWSTR)name.c_str());
}

ScreenTexture::~ScreenTexture()
{
	m_rtvDescriptor->Release();
	m_srvDescriptor->Release();
	m_resource->Release();
}
