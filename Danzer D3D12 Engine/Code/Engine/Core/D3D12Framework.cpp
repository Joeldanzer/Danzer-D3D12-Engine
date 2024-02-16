#include "stdafx.h"

#include "D3D12Framework.h"
#include "WindowHandler.h"
#include "FrameResource.h"

#include "Rendering/Screen Rendering/GBuffer.h"

#include "../3rdParty/imgui-master/backends/imgui_impl_dx12.h"
#include "../3rdParty/imgui-master/backends/imgui_impl_win32.h"

D3D12Framework::D3D12Framework() : 
	m_frameIndex(0),
	m_fenceValue(0),
	m_viewport(0.0f, 0.0f, static_cast<float>(WindowHandler::WindowData().m_w), static_cast<float>(WindowHandler::WindowData().m_h)),
	m_scissorRect(0, 0,    static_cast<LONG>(8192),  static_cast<LONG>(8192))
{
	ComPtr<IDXGIFactory4> factory;
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debuController;
	CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&debuController)));
	debuController->EnableDebugLayer();

	CHECK_HR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));
#else // Debug tool off in Release or other builds
	ComPtr<ID3D12Debug> debuController;
	CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&debuController)));
	debuController->EnableDebugLayer();
	CHECK_HR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));
#endif

	// Scan DXGI Adapters to see if it supports Direct3D 12
	ComPtr<IDXGIAdapter1> adapter;
	ScanAdapter(adapter.Get(), factory.Get());

	CHECK_HR(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;

	CHECK_HR(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount	   = FrameCount;
	swapChainDesc.Width			   = WindowHandler::WindowData().m_w;
	swapChainDesc.Height		   = WindowHandler::WindowData().m_h;
	swapChainDesc.Format		   = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> swapChain;
	CHECK_HR(factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		WindowHandler::GetHWND(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	CHECK_HR(factory->MakeWindowAssociation(WindowHandler::GetHWND(), 0));

	CHECK_HR(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create Descriptor Heaps
	{
		// Main RTV's, GBUFFER
		m_rtvHeap.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount + GBUFFER_COUNT + 1, false);
		
		// DepthStencil and ShadowMapping
		m_dsvHeap.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1 + FrameCount + 1, false);

		m_cbvSrvHeap.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_NUMBER_OF_DESCTRIPTORS, true);

		//m_samplerHeap.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 3, true);	
	}

	CHECK_HR(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	
	InitImgui();
	LoadAssets();
}

D3D12Framework::~D3D12Framework()
{
	m_rtvHeap.~DescriptorHeapWrapper();
	m_dsvHeap.~DescriptorHeapWrapper();
	m_cbvSrvHeap.~DescriptorHeapWrapper();

	m_swapChain.~ComPtr();
	m_device.~ComPtr();
	
	m_commandAllocator.~ComPtr();
	m_commandQueue.~ComPtr();
	m_initCmdList.~ComPtr();

	m_depthStencil.~ComPtr();
	m_fence.~ComPtr();

	for (UINT i = 0; i < FrameCount; i++)
	{
		m_frameResources[i]->~FrameResource();
		delete m_frameResources[i];

		m_renderTargets[i].~ComPtr();
	}
	delete m_imguiDesc;
}

void D3D12Framework::InitiateCommandList(ID3D12PipelineState* pso, std::wstring message)
{
	OutputDebugString(message.c_str());
	m_frameResources[m_frameIndex]->Initiate(pso);
}

void D3D12Framework::ExecuteCommandList()
{
	OutputDebugString(L"Executed Command List \n");

	CHECK_HR(m_frameResources[m_frameIndex]->CmdList()->Close());

	ID3D12CommandList* commandLists[] = { m_frameResources[m_frameIndex]->CmdList() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	CHECK_HR(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));

}

void D3D12Framework::WaitForGPU()
{

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	UINT64 completedValue = m_fence->GetCompletedValue();

	if (completedValue < m_fenceValue) {
		HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);
		CHECK_HR(m_fence->SetEventOnCompletion(m_fenceValue, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	m_fenceValue++;
}

void D3D12Framework::TransitionRenderTarget(D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	CD3DX12_RESOURCE_BARRIER beginTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(),
		present,
		newState
	);

	m_frameResources[m_frameIndex]->CmdList()->ResourceBarrier(1, &beginTransition);
}

void D3D12Framework::TransitionMultipleRTV(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	std::vector<CD3DX12_RESOURCE_BARRIER> transitions;
	for (UINT i = 0; i < numberOfresources; i++)
	{
		transitions.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resources[i], present, newState));
	}
	m_frameResources[m_frameIndex]->CmdList()->ResourceBarrier(numberOfresources, &transitions[0]);
}

void D3D12Framework::QeueuResourceTransition(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	for (UINT i = 0; i < numberOfresources; i++)
	{
		std::pair<ID3D12Resource*, StateTransition> transition;
		transition.first = resources[i];
		transition.second = { present, newState };
		m_transitionQeueu.emplace_back(transition);
	}
}

void D3D12Framework::TransitionAllResources()
{
	std::vector<CD3DX12_RESOURCE_BARRIER> transitionList;
	for (UINT i = 0; i < m_transitionQeueu.size(); i++)
	{
		std::pair<ID3D12Resource*, StateTransition> transition = m_transitionQeueu[i];
		transitionList.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(
			transition.first,
			transition.second.m_currentState,
			transition.second.m_newState)
		);
	}
	m_frameResources[m_frameIndex]->CmdList()->ResourceBarrier(transitionList.size(), &transitionList[0]);
	m_transitionQeueu.clear();
}

void D3D12Framework::EndInitFrame()
{
	CHECK_HR(m_initCmdList->Close());
	ID3D12CommandList* cmdList[] = { m_initCmdList.Get() };
	m_commandQueue->ExecuteCommandLists(1, cmdList);

	CHECK_HR(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValue++;

	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

	const UINT64 fenceToWaitFor = m_fenceValue;
	CHECK_HR(m_commandQueue->Signal(m_fence.Get(), fenceToWaitFor));
	m_fenceValue++;

	CHECK_HR(m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);
}

void D3D12Framework::LoadAssets()
{
	// Temporary Command List
	CHECK_HR(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_initCmdList)));

	// Create RTV's
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap.GET_CPU_DESCRIPTOR(0));
	for (UINT i = 0; i < FrameCount; i++)
	{
		CHECK_HR(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
		m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		
		rtvHandle.Offset(1, m_rtvHeap.DESCRIPTOR_SIZE());
		m_rtvHeap.m_handleCurrentOffset++;
	}

	// Create Depth Stencil
	{
		CD3DX12_RESOURCE_DESC dsvDesc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			WindowHandler::WindowData().m_w,
			WindowHandler::WindowData().m_h,
			1,
			1,
			DXGI_FORMAT_D32_FLOAT,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE
			);

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format				= DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth   = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		
		CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);

		CHECK_HR(m_device->CreateCommittedResource(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&dsvDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_depthStencil)
		));
		
		m_device->CreateDepthStencilView(m_depthStencil.Get(), nullptr, m_dsvHeap.GET_CPU_DESCRIPTOR(0));	
		m_depthStencil->SetName(L"Depth Stencil");
		m_dsvHeap.m_handleCurrentOffset++;
	}

	// Frame Resource Creation
	for (UINT i = 0; i < FrameCount; i++)		
		m_frameResources[i] = new FrameResource(m_device.Get(), i);
}

void D3D12Framework::InitImgui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WindowHandler::GetHWND());

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = MAX_NUMBER_OF_DESCTRIPTORS;
	CHECK_HR(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imguiDesc)));

	m_imguiDesc->SetName(L"ImGui Descriptor Heap");

	ImGui_ImplDX12_Init(m_device.Get(), FrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, m_imguiDesc,
		m_imguiDesc->GetCPUDescriptorHandleForHeapStart(), m_imguiDesc->GetGPUDescriptorHandleForHeapStart());
}

void D3D12Framework::ScanAdapter(IDXGIAdapter1* adapter, IDXGIFactory4* factory)
{
	HRESULT result;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++)
	{
		DXGI_ADAPTER_DESC1 desc;
		result = adapter->GetDesc1(&desc);
		if (FAILED(result)) continue;

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		// Check to see if the adapter supports Direct3D 12,
		// but don't create the actual device yet.
		result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(result)) break;
	}

#if !defined(_DEBUG)
	if (!adapter) {
		if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)))) {
			//adapter->->Reset();
		}
	}
#endif // 
}
