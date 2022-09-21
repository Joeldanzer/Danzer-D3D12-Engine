#include "stdafx.h"

#include "DirectX12Framework.h"
#include "WindowHandler.h"
#include "Engine.h"

#include "../3rdParty/imgui-master/backends/imgui_impl_dx12.h"
#include "../3rdParty/imgui-master/backends/imgui_impl_win32.h"

DirectX12Framework::DirectX12Framework() :
	m_rtvDescripterSize(0),
	m_frameIndex(0),
	m_fenceEvent(HANDLE()),
	m_fenceValues(),
	m_scissorRect(D3D12_RECT()),
	m_viewport(D3D12_VIEWPORT()),
	m_cmdIsRecording(false)
{
	InitFramework();
}
DirectX12Framework::~DirectX12Framework(){
	// Reset and execute commandList right before we close the program
	// So everything can finish correctly
	ResetCommandListAndAllocator(nullptr);
	ExecuteCommandList();
	WaitForPreviousFrame();

	m_commandList->Release();
	m_device->Release();
	m_swapChain->Release();
	m_commandQeueu->Release();
	m_depthBuffer->Release();
	m_depthDescriptor->Release();
	m_commandQeueu->Release();
	m_rtvHeap->Release();
	m_infoQueue->Release();

	for (UINT i = 0; i < FrameCount; i++)
	{
		m_renderTarget[i]->Release();
		m_fences[i]->Release();
		m_commandAllocator[i]->Release();
	}
}

void DirectX12Framework::InitFramework()
{
	HRESULT result;
	
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
	swapChainDesc.BufferDesc.Width  = WindowHandler::GetWindowData().m_width;
	swapChainDesc.BufferDesc.Height = WindowHandler::GetWindowData().m_height;
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

	// Create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	result = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	CHECK_HR(result);
	m_rtvDescripterSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create fram resource
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	ID3D12CommandAllocator** alloc = new ID3D12CommandAllocator * [FrameCount];

	for (UINT i = 0; i < FrameCount; i++)
	{
		result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTarget[i]));
		CHECK_HR(result);
		m_device->CreateRenderTargetView(m_renderTarget[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescripterSize);
	
		std::wstring name(std::to_wstring(i));
		m_renderTarget[i]->SetName((LPCWSTR)name.c_str());

		// Create command allocaters for the number of back buffers
		result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT , IID_PPV_ARGS(&m_commandAllocator[i]));
		CHECK_HR(result);

		std::wstring s = L"Command Allocator (" + std::to_wstring(i) + L")";
		m_commandAllocator[i]->SetName(s.c_str());

		//m_commandAllocator[i]->SetName(L"Direct Commandlist Allocator");
	}

	result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		m_commandAllocator[m_frameIndex].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
	CHECK_HR(result);

	m_commandList->SetName(L"Defualt CommandList");
	m_cmdIsRecording = true;
	//m_commandList->Close();
	// Immediatly close command lists as they open after being created.

	for (UINT i = 0; i < FrameCount; i++)
	{
		result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fences[i]));
		CHECK_HR(result);

		m_fenceValues[i] = 0;
	}

	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (m_fenceEvent == nullptr) {
		throw "Failed to create fence event!";
	}

	CreateDepthStencilView();
	SetViewport(WindowHandler::GetWindowData().m_width, WindowHandler::GetWindowData().m_height);

	InitImgui();

	ExecuteCommandList();
}

void DirectX12Framework::InitImgui()
{
	HRESULT result;

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WindowHandler::GetHWND());

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1000;
	result = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imguiDescriptor));
	CHECK_HR(result);

	m_imguiDescriptor->SetName(L"ImGui Descriptor Heap");

	ImGui_ImplDX12_Init(m_device.Get(), FrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, m_imguiDescriptor,
		m_imguiDescriptor->GetCPUDescriptorHandleForHeapStart(), m_imguiDescriptor->GetGPUDescriptorHandleForHeapStart());

}

void DirectX12Framework::ExecuteCommandList()
{
	assert(m_cmdIsRecording, "Trying to close a already closed CommandList");
	m_cmdIsRecording = false;

	HRESULT result = m_commandList->Close();
	CHECK_HR(result);

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
    m_commandQeueu->ExecuteCommandLists(_countof(commandLists), commandLists);

	//m_commandAllocator[m_frameIndex]->

	result = m_commandQeueu->Signal(m_fences[m_frameIndex].Get(),
		m_fenceValues[m_frameIndex]);

	CHECK_HR(result);
}


void DirectX12Framework::ResetCommandListAndAllocator(ID3D12PipelineState* pipeline)
{
	assert(!m_cmdIsRecording, "Trying to reset a recording CommandList");

	HRESULT result;
	result = m_commandAllocator[m_frameIndex]->Reset();
	CHECK_HR(result);      

	result = m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), pipeline);
	CHECK_HR(result);

	m_cmdIsRecording = true;
}

void DirectX12Framework::WaitForPreviousFrame()
{
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	
	if (m_fences[m_frameIndex]->GetCompletedValue() < m_fenceValues[m_frameIndex]) {
		HRESULT result = m_fences[m_frameIndex]->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent);
		CHECK_HR(result);

		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_fenceValues[m_frameIndex]++;
}

void DirectX12Framework::TransitionRenderTarget(D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	CD3DX12_RESOURCE_BARRIER beginTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTarget[m_frameIndex].Get(),
		present,
		newState
	);

	m_commandList->ResourceBarrier(1, &beginTransition);
}

void DirectX12Framework::TransitionMultipleRTV(ID3D12Resource** resources, UINT numberOfresources, D3D12_RESOURCE_STATES present, D3D12_RESOURCE_STATES newState)
{
	std::vector<CD3DX12_RESOURCE_BARRIER> transitions;
	for (UINT i = 0; i < numberOfresources; i++)
	{
		transitions.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resources[i], present, newState));
	}
	m_commandList->ResourceBarrier(numberOfresources, &transitions[0]);
}

void DirectX12Framework::SetViewport(UINT w, UINT h)
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

void DirectX12Framework::CreateDepthStencilView()
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_depthDescriptor));
	CHECK_HR(result);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE clearValueDesc = {};
	clearValueDesc.Format = DXGI_FORMAT_D32_FLOAT;
	clearValueDesc.DepthStencil.Depth = 1.f;
	clearValueDesc.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC tex2D = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
		WindowHandler::GetWindowData().m_width, WindowHandler::GetWindowData().m_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	result = m_device->CreateCommittedResource(
		&defaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&tex2D,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValueDesc,
		IID_PPV_ARGS(&m_depthBuffer));
	CHECK_HR(result);

	m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_depthDescriptor->GetCPUDescriptorHandleForHeapStart());
}

void DirectX12Framework::ScanAdapter(IDXGIAdapter1* adapter, IDXGIFactory4* factory)
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

D3D_FEATURE_LEVEL DirectX12Framework::GetFeatureLevel()
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

CD3DX12_CPU_DESCRIPTOR_HANDLE DirectX12Framework::GetCurrentRTVHandle()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < FrameCount; i++)
	{
		if (i == m_frameIndex)
			break;

		rtvHandle.Offset(1, m_rtvDescripterSize);
	}

	return rtvHandle;
}
