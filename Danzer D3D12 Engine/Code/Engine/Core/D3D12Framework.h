#pragma once

#include "DesriptorHeapWrapper.h"
#include <dxgi1_4.h>

class FrameResource;
class PSOHandler;

class D3D12Framework
{
public:
    struct ImguiDescHeap {
        ComPtr<ID3D12DescriptorHeap> m_descHeap;

        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle);
        void Free(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle);

        uint32_t m_handleIncrement  = 0;
        uint32_t m_maxNumberOfDescs = MAX_NUMBER_OF_DESCTRIPTORS;
        uint32_t m_currentOffset    = 0;
    };

    static ImguiDescHeap m_imguiDescHeap;

	D3D12Framework();
    ~D3D12Framework();
	
    FrameResource* CurrentFrameResource() {
        return m_frameResources[m_frameIndex];
    }
    ID3D12GraphicsCommandList* InitCmdList() {
        return m_initCmdList.Get();
    }
    
    void InitiateCommandList(ID3D12PipelineState* pso, std::wstring message);
    void ExecuteCommandList();
    void WaitForGPU();

    void TransitionRenderTarget(D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);
    void TransitionMultipleRTV(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);

    void QeueuResourceTransition(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);
    void QeueuResourceTransition(ID3D12Resource* resources,  D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState);
    void TransitionAllResources();

    void EndInitFrame(); // Called when all resources has been load and we can close InitCmdList
    bool IsInitFrame() {
        return m_initFrame;
    }

    DescriptorHeapWrapper& RTVHeap() {
        return m_rtvHeap;
    }
    DescriptorHeapWrapper& DSVHeap() {
        return m_dsvHeap;
    }
    DescriptorHeapWrapper& CbvSrvHeap() {
        return m_cbvSrvHeap;
    }

    ID3D12DescriptorHeap* GetImguiHeap() {
        return m_imguiDesc;
    }

    const UINT GetFrameIndex() {
        return m_frameIndex;
    }

    ID3D12Resource* GetCurrentRTV() {
        return m_renderTargets[m_frameIndex].Get();
    }

    ID3D12GraphicsCommandList* GetInitCmdList() {
        return m_initCmdList.Get();
    }
    ID3D12Device* GetDevice() {
        return m_device.Get();
    }
    IDXGISwapChain3* GetSwapChain() {
        return m_swapChain.Get();
    }

private:
    friend class RenderManager;

    bool m_initFrame = true;

    void RenderToBackBuffer(const uint32_t textureToPresent, PSOHandler& psoHandler);
    void SetBackBufferPSO(PSOHandler& psoHandler);

    void LoadAssets();
    void InitImgui();
    void ScanAdapter(IDXGIAdapter1*, IDXGIFactory4*);

    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT     m_scissorRect;

    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device>    m_device;
    
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12Resource> m_depthStencil;
    
    ComPtr<ID3D12CommandAllocator>    m_commandAllocator;
    ComPtr<ID3D12CommandQueue>        m_commandQueue;
    ComPtr<ID3D12GraphicsCommandList> m_initCmdList; // This commandList is only used on Initialization
    
    DescriptorHeapWrapper m_rtvHeap;
    DescriptorHeapWrapper m_dsvHeap;
    DescriptorHeapWrapper m_cbvSrvHeap;

    ID3D12DescriptorHeap* m_imguiDesc;

    FrameResource* m_frameResources[FrameCount];

    UINT   m_frameIndex;
    HANDLE m_fenceEvent;

    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    uint32_t m_backBufferPSO = UINT32_MAX;
    uint32_t m_backBufferRS  = UINT32_MAX;

    struct StateTransition {
        D3D12_RESOURCE_STATES m_currentState;
        D3D12_RESOURCE_STATES m_newState;
    };
    std::vector<std::pair<ID3D12Resource*, StateTransition>> m_transitionQeueu;
};

