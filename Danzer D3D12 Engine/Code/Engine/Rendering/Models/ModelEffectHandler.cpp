#include "stdafx.h"
#include "ModelEffectHandler.h"
#include "Core/D3D12Framework.h"
#include "Rendering/PipelineStateHandler.h"
#include "Rendering/Screen Rendering/GBuffer.h"
#include "Core/WindowHandler.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>


ModelEffectHandler::ModelEffectHandler(D3D12Framework& framework)
{
	m_device = framework.GetDevice();

	CD3DX12_RESOURCE_DESC dtDesc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		1,
		1,
		DXGI_FORMAT_R32_FLOAT,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_NONE
	);

	CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);

	CHECK_HR(m_device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&dtDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&m_depthTexture)
	));

	//framework.CbvSrvHeap().GET_CPU_DESCRIPTOR() 
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(framework.CbvSrvHeap().GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(framework.CbvSrvHeap().m_handleCurrentOffset * framework.CbvSrvHeap().DESCRIPTOR_SIZE());
	m_device->CreateShaderResourceView(m_depthTexture.Get(), nullptr, handle);
	//m_device->CreateDepthStencilView(m_depthTexture.Get(), nullptr, handle);
	m_depthTexture->SetName(L"Model Effect Depth Texture");
	m_depthTextureOffset = framework.CbvSrvHeap().m_handleCurrentOffset;
	framework.CbvSrvHeap().m_handleCurrentOffset++;
}
ModelEffectHandler::~ModelEffectHandler(){
	m_device = nullptr;
	m_modelEffects.clear();
}

ModelEffect ModelEffectHandler::CreateModelEffect(std::wstring shaderName, const UINT model, void* bufferData, const UINT sizeOfData, std::vector<UINT> textures, bool transparent)
{
	m_modelEffects.emplace_back(ModelEffectData(model, textures));
	ModelEffectData& effectData = m_modelEffects[m_modelEffects.size() - 1];

	// Create rootsignature and the buffers/textures that will be used for this effect
	std::vector<CD3DX12_ROOT_PARAMETER> rootParameter;

	rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
	CD3DX12_DESCRIPTOR_RANGE cbvDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	rootParameter[rootParameter.size() - 1].InitAsDescriptorTable(1, &cbvDescriptorRange);

	if (bufferData) { 
		rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
		CD3DX12_DESCRIPTOR_RANGE bufferDescRanger(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
		rootParameter[rootParameter.size() - 1].InitAsDescriptorTable(1, &bufferDescRanger);
	}

	std::array<CD3DX12_DESCRIPTOR_RANGE, 10> descriptorRanges;
	for (UINT i = 0; i < textures.size() + 1; i++)
	{
		rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
		descriptorRanges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i); 
		rootParameter[rootParameter.size() - 1].InitAsDescriptorTable(1, &descriptorRanges[i], D3D12_SHADER_VISIBILITY_ALL); // Shader visibility will be set to all
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(rootParameter.size(), &rootParameter[0], 1, &PipelineStateHandler::s_samplerDescs[SAMPLER_DESC_WRAP],
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	ID3DBlob* signature = nullptr;
	ID3DBlob* error     = nullptr;

	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	CHECK_HR(result);
	CHECK_HR(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&effectData.m_rootSignature)));

	effectData.m_rootSignature->SetName(std::wstring(shaderName + L" Root Signature").c_str());

	std::array<DXGI_FORMAT, GBUFFER_COUNT> formats = {
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* ALBEDO
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* NORMAL
		DXGI_FORMAT_R8G8B8A8_UNORM,		 //* MATERIAL
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* VERTEX COLOR
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* VERTEX NORMAL
		DXGI_FORMAT_R32G32B32A32_FLOAT,  //* WORLD POSITION
		DXGI_FORMAT_R32_FLOAT			 //* DEPTH TEXTURE
	};

	// Create PipelineState for Model effect
	D3D12_BLEND_DESC blendDesc		  = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	for (UINT i = 3; i < formats.size(); i++)
	{
		blendDesc.RenderTarget[i].BlendEnable = true;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[i].DestBlend = D3D12_BLEND_BLEND_FACTOR;
		blendDesc.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.AlphaToCoverageEnable = false;
	}

	D3D12_RASTERIZER_DESC    rasterizerDesc   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	// Will implement a check for both Vertex And Pixel shader if it should run the default one or not
	CHECK_HR(D3DReadFileToBlob(std::wstring(L"Shaders/" + shaderName + L"VS.cso").c_str(), &vs));
	CHECK_HR(D3DReadFileToBlob(std::wstring(L"Shaders/" + shaderName + L"PS.cso").c_str(), &ps));

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	for (UINT i = 0; i < formats.size(); i++)
		psoDesc.RTVFormats[i] = formats[i];
	
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.NumRenderTargets = formats.size();
	psoDesc.pRootSignature = effectData.m_rootSignature.Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = PipelineStateHandler::s_inputLayouts[INPUT_LAYOUT_INSTANCE_FORWARD].data();
	psoDesc.InputLayout.NumElements        = PipelineStateHandler::s_inputLayouts[INPUT_LAYOUT_INSTANCE_FORWARD].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	CHECK_HR(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&effectData.m_pipelineState)));

	effectData.m_pipelineState->SetName(L"Effect Model PSO");

	return ModelEffect(m_modelEffects.size());
}
