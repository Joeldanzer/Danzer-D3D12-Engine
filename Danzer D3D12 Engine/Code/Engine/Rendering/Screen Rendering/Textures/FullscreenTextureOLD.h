#pragma once
#include "DirectX/include/directx/d3dx12.h"
#include "Core/MathDefinitions.h"
#include "Core/D3D12Header.h"

class D3D12Framework;
class DescriptorHeapWrapper;
class TextureHandler;
class PSOHandler;

class FullscreenTextureOLD
{
public:
	FullscreenTextureOLD() : 
		m_dsvOffsetID(0), m_srvOffsetID(0), m_viewPort({}), m_pso(0), m_rs(0), m_rtvOffsetID(0)
	{}
	~FullscreenTextureOLD();

	void InitAsDepth(
		ID3D12Device* device,
		DescriptorHeapWrapper* cbvSrvHeap,
		DescriptorHeapWrapper* dsvHeap,
		const UINT width,
		const UINT height,
		DXGI_FORMAT textureDesc,
		DXGI_FORMAT srvFormat,
		D3D12_RESOURCE_FLAGS flag,
		std::wstring name
	);
	void InitAsTexture(
		ID3D12Device* device,
		DescriptorHeapWrapper* cbvSrvHeap,
		DescriptorHeapWrapper* rtvHeap,
		const UINT width,
		const UINT height,
		DXGI_FORMAT textureDesc,
		DXGI_FORMAT srvFormat,
		D3D12_RESOURCE_FLAGS flag,
		std::wstring name
	);

	virtual void InitBuffers(ID3D12Device* device, DescriptorHeapWrapper& cbvWrapper);
	virtual void SetPipelineAndRootSignature(PSOHandler& psoHandler) {};

	void SetPipelineAndRootSignature(
		std::wstring			   vertexShader,
		std::wstring			   pixelShader,
		bool					   depthEnabled,
		DXGI_FORMAT				   format,
		const uint8_t			   blendDesc,
		const uint8_t			   rastDesc,
		const uint8_t			   samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAGS flags,
		DXGI_FORMAT				   depthFormat,
		const uint8_t			   numberOfBuffers,
		const uint8_t			   numberOfTextures,
		const uint8_t			   inputLayout,
		std::wstring			   textureName,
		PSOHandler&				   psoHandler
	);


	void SetAsRenderTarget(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* rtvWrapper, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle, const UINT frameIndex);
	void SetViewportAndPSO(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler);
	
	void SetTextureAtSlot(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* srvWrapper, const UINT slot, const UINT frameIndex);
	//void SetTextureAtSlot(DescriptorHeapWrapper* srvWrapper, const uint16_t slot);
	virtual void RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex){}

	ID3D12Resource* GetResource(const UINT frameIndex) {
		return m_resource[frameIndex].Get();
	}

	const UINT SRVOffsetID() {
		return m_srvOffsetID;
	}
	const UINT DSVOffsetID() {
		return m_dsvOffsetID;
	}
	const UINT RTVOffsetID() {
		return m_rtvOffsetID;
	}
	const D3D12_VIEWPORT& GetViewPort() {
		return m_viewPort;
	}
	const UINT GetPSO() {
		return m_pso;
	}
	const UINT GetRootSignature() {
		return m_rs;
	}

	enum class TextureType {
		RTV_TEXTURE,
		DSV_TEXTURE
	};

protected:
	friend class TextureRenderingHandler;

	TextureType m_textureType;

	UINT m_dsvOffsetID;
	UINT m_srvOffsetID; 
	UINT m_rtvOffsetID;

	UINT m_pso;
	UINT m_rs;

	D3D12_VIEWPORT m_viewPort;
	ComPtr<ID3D12Resource> m_resource[FrameCount];
};

