#pragma once

#ifndef DIRECT_X12_FRAMEWORK
#define DIRECT_X12_FRAMEWORK

#include "DesriptorHeapWrapper.h"
#include <dxgi1_4.h>

class DirectX12Framework
{
public:
	DirectX12Framework();
	~DirectX12Framework();

	void InitFramework();

	void ExecuteCommandList(); 

	void ResetCommandListAndAllocator(ID3D12PipelineState* pipeline);
	void WaitForPreviousFrame();
	
	void TransitionRenderTarget(D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);
	void TransitionMultipleRTV(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);

	void QeueuResourceTransition(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);
	void TransitionAllResources(); 

	void SetViewport(UINT w, UINT h);

	D3D_FEATURE_LEVEL GetFeatureLevel();

	const UINT	  GetFrameIndex()  { return m_frameIndex; }
	const UINT	  GetRTVDescSize() { return m_rtvDescripterSize; }
	const UINT64  GetFenceValue()  { return m_fenceValues[m_frameIndex]; }

	D3D12_RECT* GetRect() { return &m_scissorRect; }

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();

	DescriptorHeapWrapper& GetCbvSrvUavWrapper() { return m_cbvSrvUavWrapper; }

	ID3D12Device*			   GetDevice()		   { return m_device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList()    { return m_commandList.Get(); }
	ID3D12Resource*			   GetRTV()			   { return m_renderTarget[m_frameIndex].Get(); }
	ID3D12DescriptorHeap*	   GetRTVHeap()		   { return m_rtvHeap.Get(); }
	//ID3D12DescriptorHeap*	   GetDescriptorHeap() { return m_rtvHeap.Get(); }
	ID3D12DescriptorHeap*	   GetImguiHeap()	   { return m_imguiDescriptor; }
	IDXGISwapChain3*		   GetSwapChain()	   { return m_swapChain.Get(); }
	ID3D12CommandQueue*		   GetCommandQeueu()   { return m_commandQeueu.Get(); }
	ID3D12Fence*			   GetFence()		   { return m_fences[m_frameIndex].Get(); }
	

	bool CmdListIsRecording() const noexcept {
		return m_cmdIsRecording;
	}

private:
	
	void ScanAdapter(IDXGIAdapter1*, IDXGIFactory4*);
	void CreateDepthStencilView();
	void InitImgui();
	
	friend class RenderManager;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	// Def wanna spli this class up. Tho DX12Framework can be used to init everything still
	ComPtr<IDXGISwapChain3>			  m_swapChain;
	ComPtr<ID3D12Device>			  m_device;
	
	struct StateTransition {
		D3D12_RESOURCE_STATES m_currentState;
		D3D12_RESOURCE_STATES m_newState;
	};
	std::vector<std::pair<ID3D12Resource*, StateTransition>> m_transitionQeueu;
	
	// DescriptorHeaps wrappers
	DescriptorHeapWrapper			  m_cbvSrvUavWrapper;
	DescriptorHeapWrapper			  m_samplerWrapper;
	DescriptorHeapWrapper			  m_rtvWrapper;
	DescriptorHeapWrapper			  m_dsvWrapper;

	ComPtr<ID3D12Resource>			  m_depthBuffer;
	ComPtr<ID3D12DescriptorHeap>	  m_depthDescriptor;
	
	ID3D12DescriptorHeap*			  m_imguiDescriptor;


	ComPtr<ID3D12CommandQueue>		  m_commandQeueu;
	ComPtr<ID3D12CommandAllocator>	  m_commandAllocator[FrameCount];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	ComPtr<ID3D12DescriptorHeap>	  m_rtvHeap;
	ComPtr<ID3D12Resource>			  m_renderTarget[FrameCount];
	ComPtr<ID3D12Fence>				  m_fences[FrameCount];
	ComPtr<ID3D12InfoQueue>			  m_infoQueue;

	HANDLE m_fenceEvent;			 
	UINT64 m_fenceValues[FrameCount];

	bool m_cmdIsRecording;

	UINT							  m_rtvDescripterSize;
	UINT						      m_frameIndex;
};

#endif
