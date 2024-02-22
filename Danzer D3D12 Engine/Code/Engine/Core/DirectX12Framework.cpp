#include "stdafx.h"

#include "DirectX12Framework.h"
#include "WindowHandler.h"
#include "Engine.h"
#include "Rendering/Screen Rendering/GBuffer.h"

#include "../3rdParty/imgui-master/backends/imgui_impl_dx12.h"
#include "../3rdParty/imgui-master/backends/imgui_impl_win32.h"

DirectX12FrameworkOLD::DirectX12FrameworkOLD() :
	m_rtvDescripterSize(0),
	m_frameIndex(0),
	m_fenceEvent(HANDLE()),
	m_fenceValue(0),
	m_scissorRect(D3D12_RECT()),
	m_viewport(D3D12_VIEWPORT()),
	m_cmdIsRecording(false)
{
	InitFramework();
}
DirectX12FrameworkOLD::~DirectX12FrameworkOLD(){
	ExecuteCommandList();
	WaitForPreviousFrame();

	m_commandList->Release();
	m_device->Release();
	m_swapChain->Release();
	m_commandQeueu->Release();
	m_depthDescriptor->Release();
	m_commandQeueu->Release();
	m_infoQueue->Release();
	m_fence->Release();
    m_commandAllocator->Release();

	for (UINT i = 0; i < FrameCount; i++)
	{
		m_depthBuffer[i]->Release();
		m_renderTarget[i]->Release();
	}
}

void DirectX12FrameworkOLD::InitFramework()
{
	HRESULT result;
	
	SetViewport(WindowHandler::WindowData().m_w, WindowHandler::WindowData().m_h);
	
	ComPtr<IDXGIFactory4> factory;
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debuController;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debuController));
	CHECK_HR(result);	
	debuController->EnableDebugLayer();
	
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else // Debug tool off in Release or other builds
	ComPtr<ID3D12Debug> debuController;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debuController));
	CHECK_HR(result);
	debuController->EnableDebugLayer();
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#endif
	CHECK_HR(result);

	// Scan DXGI Adapters to see if it supports Direct3D 12
	ComPtr<IDXGIAdapter1> adapter;
	ScanAdapter(adapter.Get(), factory.Get());

	result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
	CHECK_HR(result);

	m_device->QueryInterface(IID_PPV_ARGS(&m_infoQueue));
	m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);

	D3D12_COMMAND_QUEUE_DESC qeueuDesc = {};
	qeueuDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	qeueuDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = m_device->CreateCommandQueue(&qeueuDesc, IID_PPV_ARGS(&m_commandQeueu));
	CHECK_HR(result);

	m_commandQeueu->SetName(L"Default CommandQeueu");

	DXGI_SAMPLE_DESC sampleDesc = {1, 0};

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount		= FrameCount;
	swapChainDesc.BufferDesc.Width  = WindowHandler::WindowData().m_w;
	swapChainDesc.BufferDesc.Height = WindowHandler::WindowData().m_h;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage	    = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect	    = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow	    = WindowHandler::GetHWND();
	swapChainDesc.SampleDesc		= sampleDesc;
	swapChainDesc.Windowed		    = true;

	ComPtr<IDXGISwapChain> swapChain;
	result = factory->CreateSwapChain(m_commandQeueu.Get(), &swapChainDesc, &swapChain);
	CHECK_HR(result);

	result = swapChain.As(&m_swapChain);
	CHECK_HR(result);

	result = factory->MakeWindowAssociation(WindowHandler::GetHWND(), DXGI_MWA_VALID);
	CHECK_HR(result);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// RTV's, GBUFFER, ShadowMapping
	m_rtvWrapper.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount + (GBUFFER_COUNT) + 1, false);

	CreateDepthStencilView();
	InitImgui();
	
	for (UINT i = 0; i < FrameCount; i++)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvWrapper.GET_CPU_DESCRIPTOR(i));
		result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTarget[i]));
		CHECK_HR(result);
		m_device->CreateRenderTargetView(m_renderTarget[i].Get(), nullptr, rtvHandle);
		m_rtvWrapper.m_handleCurrentOffset++;
	
		std::wstring name(std::to_wstring(i));
		m_renderTarget[i]->SetName((LPCWSTR)name.c_str());
	}
	
	result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT , IID_PPV_ARGS(&m_commandAllocator));
	CHECK_HR(result);

	std::wstring s = L"Default Command Allocator";
	m_commandAllocator->SetName(s.c_str());

	result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
	CHECK_HR(result);

	m_commandList->SetName(L"Defualt CommandList");
	
	result = m_commandList->Close();
	CHECK_HR(result);

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQeueu->ExecuteCommandLists(_countof(commandLists), commandLists);

	// Creates the descriptor heap used for all the information that gets sent to the GPU before rendering.
	m_cbvSrvUavWrapper.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, FrameCount + MAX_NUMBER_OF_DESCTRIPTORS + 2, true);

	// Fences 
	result = m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));	
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

	const UINT64 fenceToWaitFor = m_fenceValue;
	result = m_commandQeueu->Signal(m_fence.Get(), fenceToWaitFor);
	m_fenceValue++;

	result = m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
	WaitForSingleObject(m_fenceEvent, INFINITE);
}

void DirectX12FrameworkOLD::InitImgui()
{
	HRESULT result;

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WindowHandler::GetHWND());

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 10000;
	result = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imguiDescriptor));
	CHECK_HR(result);

	m_imguiDescriptor->SetName(L"ImGui Descriptor Heap");

	ImGui_ImplDX12_Init(m_device.Get(), FrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, m_imguiDescriptor,
		m_imguiDescriptor->GetCPUDescriptorHandleForHeapStart(), m_imguiDescriptor->GetGPUDescriptorHandleForHeapStart());
}

void DirectX12FrameworkOLD::ExecuteCommandList()
{
	assert(m_cmdIsRecording, "Trying to close a already closed CommandList");
	m_cmdIsRecording = false;

	HRESULT result = m_commandList->Close();
	CHECK_HR(result);

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
    m_commandQeueu->ExecuteCommandLists(_countof(commandLists), commandLists);

	result = m_commandQeueu->Signal(m_fence.Get(), m_fenceValue);
	CHECK_HR(result);

	OutputDebugString(L"Command list succesfully closed \n");
}


void DirectX12FrameworkOLD::ResetCommandListAndAllocator(ID3D12PipelineState* pipeline, std::wstring debugText)
{
	debugText += L"\n";
	OutputDebugString(debugText.c_str());
	
	//assert(!m_cmdIsRecording, "Trying to reset a recording CommandList");
	if (!m_cmdIsRecording) {
		HRESULT result;
		result = m_commandAllocator->Reset();
		CHECK_HR(result);      

		result = m_commandList->Reset(m_commandAllocator.Get(), pipeline);
		CHECK_HR(result);

		m_cmdIsRecording = true;
	}
}

void DirectX12FrameworkOLD::WaitForPreviousFrame()
{
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	
	UINT64 completedValue = m_fence->GetCompletedValue();
	
	if (completedValue < m_fenceValue) {
		HRESULT result = m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		CHECK_HR(result);
	
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
	
	m_fenceValue++;
}

void DirectX12FrameworkOLD::TransitionRenderTarget(D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	CD3DX12_RESOURCE_BARRIER beginTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTarget[m_frameIndex].Get(),
		present,
		newState
	);

	m_commandList->ResourceBarrier(1, &beginTransition);
}

void DirectX12FrameworkOLD::TransitionMultipleRTV(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	std::vector<CD3DX12_RESOURCE_BARRIER> transitions;
	for (UINT i = 0; i < numberOfresources; i++)
	{
		transitions.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resources[i], present, newState));
	}
	m_commandList->ResourceBarrier(numberOfresources, &transitions[0]);
}

void DirectX12FrameworkOLD::QeueuResourceTransition(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	for (UINT i = 0; i < numberOfresources; i++)
	{
		std::pair<ID3D12Resource*, StateTransition> transition;
		transition.first = resources[i];
		transition.second = {present, newState};
		m_transitionQeueu.emplace_back(transition);
	}
}

void DirectX12FrameworkOLD::TransitionAllResources()
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
	m_commandList->ResourceBarrier(transitionList.size(), &transitionList[0]);	
	m_transitionQeueu.clear();
}

void DirectX12FrameworkOLD::SetViewport(UINT w, UINT h)
{
	s_engineState = EngineState::ENGINE_STATE_GAME;
	switch (s_engineState)
	{
	case EngineState::ENGINE_STATE_EDITOR:
		// Minimizes ouput Window and centers it 

	    // Define viewport
		m_viewport.TopLeftX = w - (w / 2);
		m_viewport.TopLeftY = 19;
		m_viewport.Width  = (float)w;
		m_viewport.Height = (float)h + 19;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		
		// Define scissor rect
		m_scissorRect.left = w - (w / 2);
		m_scissorRect.top = 19;
		m_scissorRect.right = w + (w / 2);
		m_scissorRect.bottom = h + 19;
		break;
	case EngineState::ENGINE_STATE_GAME:
	    // Define viewport
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width  = (float)w;
		m_viewport.Height = (float)h;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		// Define scissor rect
		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = w;
		m_scissorRect.bottom = h;
		break;
	default:
		break;
	}
}

void DirectX12FrameworkOLD::CreateDepthStencilView()
{
	HRESULT result;

	m_dsvWrapper.CreateDescriptorHeap(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, FrameCount, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE clearValueDesc = {};
	clearValueDesc.Format = DXGI_FORMAT_D32_FLOAT;
	clearValueDesc.DepthStencil.Depth = 1.0f;
	clearValueDesc.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC tex2D = CD3DX12_RESOURCE_DESC(
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
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	for (UINT i = 0; i < FrameCount; i++)
	{
		result = m_device->CreateCommittedResource(
			&defaultHeap,
			D3D12_HEAP_FLAG_NONE,
			&tex2D,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValueDesc,
			IID_PPV_ARGS(&m_depthBuffer[i]));
		CHECK_HR(result);

		m_device->CreateDepthStencilView(m_depthBuffer[i].Get(), &dsvDesc, m_dsvWrapper.GET_CPU_DESCRIPTOR(i));
	}
}

void DirectX12FrameworkOLD::ScanAdapter(IDXGIAdapter1* adapter, IDXGIFactory4* factory)
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

D3D_FEATURE_LEVEL DirectX12FrameworkOLD::GetFeatureLevel()
{
	static const D3D_FEATURE_LEVEL s_featureLevel[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		_countof(s_featureLevel), s_featureLevel, D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT result = m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	if (SUCCEEDED(result)) {
		
		featureLevel = featLevels.MaxSupportedFeatureLevel;
	}

	return featureLevel;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DirectX12FrameworkOLD::GetCurrentRTVHandle()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvWrapper.GET_CPU_DESCRIPTOR(m_frameIndex));
	return rtvHandle;
}
