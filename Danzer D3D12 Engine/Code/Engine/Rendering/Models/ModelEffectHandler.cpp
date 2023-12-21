#include "stdafx.h"
#include "ModelEffectHandler.h"
#include "Core/D3D12Framework.h"
#include "Rendering/PipelineStateHandler.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>


ModelEffectHandler::ModelEffectHandler(D3D12Framework& framework)
{
	m_device = framework.GetDevice();
}
ModelEffectHandler::~ModelEffectHandler(){
	m_device = nullptr;
	m_modelEffects.clear();
}

ModelEffect ModelEffectHandler::CreateModelEffect(std::wstring shaderName, const UINT model, const UINT numberOfBuffers, std::vector<UINT> textures, bool transparent)
{
	//ModelEffectData effectData(model, textures);
	m_modelEffects.emplace_back(ModelEffectData(model, textures));
	ModelEffectData& effectData = m_modelEffects[m_modelEffects.size() - 1];

	// Create rootsignature and the buffers/textures that will be used for this effect
	std::vector<CD3DX12_ROOT_PARAMETER> rootParameter;

	rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
	CD3DX12_DESCRIPTOR_RANGE cbvDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	rootParameter[rootParameter.size() - 1].InitAsDescriptorTable(1, &cbvDescriptorRange);

	for (UINT i = 0; i < numberOfBuffers; i++)
	{
		rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
		CD3DX12_DESCRIPTOR_RANGE bufferDescRanger(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1 + i);
		rootParameter[rootParameter.size() - 1].InitAsDescriptorTable(1, &bufferDescRanger);
	}

	for (UINT i = 0; i < textures.size(); i++)
	{
		rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
		CD3DX12_DESCRIPTOR_RANGE srvDescriptorRange1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameter[rootParameter.size() - 1].InitAsDescriptorTable(1, &srvDescriptorRange1, D3D12_SHADER_VISIBILITY_ALL); // Shader visibility will be set to all
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(rootParameter.size(), &rootParameter[0], 1, &PipelineStateHandler::s_samplerDescs[PipelineStateHandler::SAMPLER_DESC_WRAP],
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;

	CHECK_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	CHECK_HR(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&effectData.m_rootSignature)));

	effectData.m_rootSignature->SetName(std::wstring(shaderName + L" Root Signature").c_str());

	// Create PipelineState for Model effect
	HRESULT result;

	D3D12_BLEND_DESC		 blendDesc		  = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_RASTERIZER_DESC    rasterizerDesc   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
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
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.NumRenderTargets = 1;
	psoDesc.pRootSignature = effectData.m_rootSignature.Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = PipelineStateHandler::s_inputLayouts[PipelineStateHandler::INPUT_LAYOUT_INSTANCE_MODEL].data();
	psoDesc.InputLayout.NumElements        = PipelineStateHandler::s_inputLayouts[PipelineStateHandler::INPUT_LAYOUT_INSTANCE_MODEL].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	CHECK_HR(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&effectData.m_pipelineState)));

	effectData.m_pipelineState->SetName(L"Model PSO");

	m_modelEffects.emplace_back(effectData);

	return ModelEffect(m_modelEffects.size());
}
