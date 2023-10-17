#include "stdafx.h"
#include "PipelineStateHandler.h"
#include "Core/DirectX12Framework.h"
#include "Screen Rendering/GBuffer.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

PipelineStateHandler::PipelineStateHandler(){}
PipelineStateHandler::~PipelineStateHandler(){

	for (UINT i = 0; i < PIPELINE_COUNT; i++)
	{
		m_PSObjects[i].~ComPtr();
	}

	for (UINT i = 0; i < ROOTSIGNATURE_COUNT; i++)
	{
		m_rootSignatures[i].~ComPtr();
	}
	
}

void PipelineStateHandler::Init(DirectX12Framework& framework)
{
	InitializeInputLayouts();
	InitializeSamplerDescs();

	CreateUIRootSignature(framework.GetDevice());
	CreateLightRootSignature(framework.GetDevice());
	CreateDefaultRootSingature(framework.GetDevice());
	CreateGBufferRootSingature(framework.GetDevice());

	ID3D12Device* device = framework.GetDevice();

	CreateUIPSO(device);
	CreateFontPSO(device);
	CreateModelPSO(device);
	CreateSkyboxPSO(device);
	CreateGBufferPSO(device);
	CreateTransparentPSO(device);
	CreateRayWireframePSO(device);
	CreateAABBWireframePSO(device);
	CreateDirectionalLightPSO(device);
}

//* Light Root Signature
void PipelineStateHandler::CreateLightRootSignature(ID3D12Device* device)
{
	CD3DX12_ROOT_PARAMETER rootParameter[9] = {};
	
	//* Camera & Light Buffer
	CD3DX12_DESCRIPTOR_RANGE cameraLightBuffer(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	rootParameter[0].InitAsDescriptorTable(1, &cameraLightBuffer, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_DESCRIPTOR_RANGE lightBuffer(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	rootParameter[1].InitAsDescriptorTable(1, &lightBuffer, D3D12_SHADER_VISIBILITY_PIXEL);

	//* Albedo, Normal, Material, VertexColor, VertexNormal, WorldPosition, Skybox 
	CD3DX12_DESCRIPTOR_RANGE albedo(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	rootParameter[2].InitAsDescriptorTable(1, &albedo, D3D12_SHADER_VISIBILITY_PIXEL);
	CD3DX12_DESCRIPTOR_RANGE normal(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	rootParameter[3].InitAsDescriptorTable(1, &normal, D3D12_SHADER_VISIBILITY_PIXEL);
	CD3DX12_DESCRIPTOR_RANGE material(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	rootParameter[4].InitAsDescriptorTable(1, &material, D3D12_SHADER_VISIBILITY_PIXEL);
	CD3DX12_DESCRIPTOR_RANGE vertexColor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	rootParameter[5].InitAsDescriptorTable(1, &vertexColor, D3D12_SHADER_VISIBILITY_PIXEL);
	CD3DX12_DESCRIPTOR_RANGE vertexNormal(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
	rootParameter[6].InitAsDescriptorTable(1, &vertexNormal, D3D12_SHADER_VISIBILITY_PIXEL);
	CD3DX12_DESCRIPTOR_RANGE worldPosition(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
	rootParameter[7].InitAsDescriptorTable(1, &worldPosition, D3D12_SHADER_VISIBILITY_PIXEL);
	CD3DX12_DESCRIPTOR_RANGE skybox(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
	rootParameter[8].InitAsDescriptorTable(1, &skybox, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameter), &rootParameter[0], 1, &m_samplerDescs[SAMPLER_DESC_WRAP], 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS     |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS       |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS     
	);

	ID3DBlob* signature = nullptr;
	ID3DBlob* error     = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, & error);
	CHECK_HR(result);

	result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[ROOTSIGNATURE_STATE_LIGHT]));
	CHECK_HR(result);

	m_rootSignatures[ROOTSIGNATURE_STATE_LIGHT]->SetName(L"Light Root Signature");
}
//* GBuffer Root signature
void PipelineStateHandler::CreateGBufferRootSingature(ID3D12Device* device)
{
	CD3DX12_ROOT_PARAMETER rootParameter[8] = {};

	//* Camera Buffer
	CD3DX12_DESCRIPTOR_RANGE cbvCamera(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	rootParameter[0].InitAsDescriptorTable(1, &cbvCamera);

	CD3DX12_DESCRIPTOR_RANGE cbvMaterial(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	rootParameter[1].InitAsDescriptorTable(1, &cbvMaterial, D3D12_SHADER_VISIBILITY_PIXEL);

	//* Albedo texture
	CD3DX12_DESCRIPTOR_RANGE albedoDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	rootParameter[2].InitAsDescriptorTable(1, &albedoDescRange, D3D12_SHADER_VISIBILITY_PIXEL);

	//* Normal texture
	CD3DX12_DESCRIPTOR_RANGE normalDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	rootParameter[3].InitAsDescriptorTable(1, &normalDescRange, D3D12_SHADER_VISIBILITY_PIXEL);
	
	//* Metallic texture
	CD3DX12_DESCRIPTOR_RANGE metallicDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	rootParameter[4].InitAsDescriptorTable(1, &metallicDescRange, D3D12_SHADER_VISIBILITY_PIXEL);

	//* Roughness texture
	CD3DX12_DESCRIPTOR_RANGE roguhnessDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	rootParameter[5].InitAsDescriptorTable(1, &roguhnessDescRange, D3D12_SHADER_VISIBILITY_PIXEL);

	//* Height texture
	CD3DX12_DESCRIPTOR_RANGE heightDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
	rootParameter[6].InitAsDescriptorTable(1, &heightDescRange, D3D12_SHADER_VISIBILITY_PIXEL);

	//* Height texture
	CD3DX12_DESCRIPTOR_RANGE aoDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
	rootParameter[7].InitAsDescriptorTable(1, &aoDescRange, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameter), &rootParameter[0], 1, &m_samplerDescs[SAMPLER_DESC_WRAP],
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	CHECK_HR(result);

	result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[ROOTSIGNATURE_STATE_GBUFFER]));
	CHECK_HR(result);

	m_rootSignatures[ROOTSIGNATURE_STATE_GBUFFER]->SetName(L"GBuffer Root Signature");
}
void PipelineStateHandler::CreateDefaultRootSingature(ID3D12Device* device)
{
	CD3DX12_ROOT_PARAMETER rootParameter[3] = {};

	CD3DX12_DESCRIPTOR_RANGE cbvDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	rootParameter[0].InitAsDescriptorTable(1, &cbvDescriptorRange);
	
	CD3DX12_DESCRIPTOR_RANGE srvDescriptorRange1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	rootParameter[1].InitAsDescriptorTable(1, &srvDescriptorRange1, D3D12_SHADER_VISIBILITY_PIXEL);
	
	CD3DX12_DESCRIPTOR_RANGE srvDescriptorRange2(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	rootParameter[2].InitAsDescriptorTable(1, &srvDescriptorRange2, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameter), &rootParameter[0], 1, &m_samplerDescs[SAMPLER_DESC_WRAP],
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS	 |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	CHECK_HR(result);

	result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT]));
	CHECK_HR(result);

	m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT]->SetName(L"Default Root Signature");
}
void PipelineStateHandler::CreateUIRootSignature(ID3D12Device* device)
{
	CD3DX12_ROOT_PARAMETER rootParameter[2] = {};

	// Window Size buffe
	CD3DX12_DESCRIPTOR_RANGE cbvDescriptorRange = {};

	cbvDescriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // CBV0
	rootParameter[0].InitAsDescriptorTable(1, &cbvDescriptorRange);

	// SRV Root singature creation
	CD3DX12_DESCRIPTOR_RANGE srvDescriptorRange = {};
	srvDescriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // SRV0
	rootParameter[1].InitAsDescriptorTable(1, &srvDescriptorRange);
	// end of SRV

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameter), rootParameter, 1, &m_samplerDescs[SAMPLER_DESC_CLAMP],
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS	 |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	CHECK_HR(result);

	result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[ROOTSIGNATURE_STATE_UI]));
	CHECK_HR(result);

	m_rootSignatures[ROOTSIGNATURE_STATE_UI]->SetName(L"UI Root Signature");
}

void PipelineStateHandler::CreateTransparentPSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.AlphaToCoverageEnable = false;
	
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthStencilDesc.DepthEnable = false;

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/vertexShader.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/pixelShader.cso", &ps);
	CHECK_HR(result);

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
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_TRANSPARENT]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_TRANSPARENT]->SetName(L"Transparent PSO");
}
void PipelineStateHandler::CreateAABBWireframePSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthStencilDesc.DepthEnable = false;
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs; ID3DBlob* gs ;ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/AABBVS.cso", &vs);
	CHECK_HR(result);
	
	result = D3DReadFileToBlob(L"Shaders/AABBGS.cso", &gs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/ColliderPS.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE gsByte = {};
	gsByte.BytecodeLength = gs->GetBufferSize();
	gsByte.pShaderBytecode = gs->GetBufferPointer();
	
	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.NumRenderTargets = 1;
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT].Get();
	psoDesc.VS = vsByte;
	psoDesc.GS = gsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_AABB].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_AABB].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_AABB_WIREFRAME]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_MODELS]->SetName(L"AABB Wireframe PSO");
}
void PipelineStateHandler::CreateRayWireframePSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthStencilDesc.DepthEnable = false;
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs; ID3DBlob* gs; ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/RayVS.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/RayGS.cso", &gs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/ColliderPS.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE gsByte = {};
	gsByte.BytecodeLength = gs->GetBufferSize();
	gsByte.pShaderBytecode = gs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.NumRenderTargets = 1;
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT].Get();
	psoDesc.VS = vsByte;
	psoDesc.GS = gsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_RAY].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_RAY].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_RAY_WIREFRAME]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_MODELS]->SetName(L"Ray Wireframe PSO");
}
void PipelineStateHandler::CreateDirectionalLightPSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/FullscreenVS.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/DirectionalLightPS.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.NumRenderTargets = 1;
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_LIGHT].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_DIRECTIONAL_LIGHT]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_DIRECTIONAL_LIGHT]->SetName(L"Directional Light PSO");
}
void PipelineStateHandler::CreateModelPSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/vertexShader.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/pixelShader.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength  = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength  = ps->GetBufferSize();
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
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_MODELS]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_MODELS]->SetName(L"Model PSO");
}
void PipelineStateHandler::CreateUIPSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blendDesc.RenderTarget->BlendEnable = true;
	blendDesc.RenderTarget->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget->SrcBlend = D3D12_BLEND_SRC_COLOR;
	blendDesc.RenderTarget->DestBlend = D3D12_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget->BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget->BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget->SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget->DestBlendAlpha = D3D12_BLEND_ZERO;

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/uiVertexShader.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/uiPixelShader.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength  = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength  = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.NumRenderTargets = 1;
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_UI].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_SPRITE_2D].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_SPRITE_2D].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_UI]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_UI]->SetName(L"UI PSO");
}
void PipelineStateHandler::CreateFontPSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blendDesc.RenderTarget->BlendEnable = true;
	blendDesc.RenderTarget->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget->SrcBlend = D3D12_BLEND_SRC_COLOR;
	blendDesc.RenderTarget->DestBlend = D3D12_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget->BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget->BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget->SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget->DestBlendAlpha = D3D12_BLEND_ZERO;

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* gs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/FontVertexShader.cso", &vs);
	CHECK_HR(result);
	
	result = D3DReadFileToBlob(L"Shaders/FontGeometryShader.cso", &gs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/FontPixelShader.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE gsByte = {};
	gsByte.BytecodeLength = gs->GetBufferSize();
	gsByte.pShaderBytecode = gs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.NumRenderTargets = 1;
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_UI].Get();
	psoDesc.VS = vsByte;
	psoDesc.GS = gsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_FONT_2D].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_FONT_2D].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_FONT]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_FONT]->SetName(L"FONT PSO");
}
void PipelineStateHandler::CreateSkyboxPSO(ID3D12Device* device)
{

	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/skyboxVS.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/skyboxPS.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength  = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength  = ps->GetBufferSize();
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
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_DEFAULT].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_SKYBOX]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_MODELS]->SetName(L"Model PSO");
}
void PipelineStateHandler::CreateGBufferPSO(ID3D12Device* device)
{
	HRESULT result;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blendDesc.RenderTarget->BlendEnable = false;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//depthStencilDesc.DepthEnable = true;
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//depthStencilDesc.StencilEnable = true;

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	DXGI_SAMPLE_DESC sample = { 1, 0 };

	ID3DBlob* vs;
	ID3DBlob* ps;

	result = D3DReadFileToBlob(L"Shaders/GbufferVS.cso", &vs);
	CHECK_HR(result);

	result = D3DReadFileToBlob(L"Shaders/GbufferPS.cso", &ps);
	CHECK_HR(result);

	D3D12_SHADER_BYTECODE vsByte = {};
	vsByte.BytecodeLength  = vs->GetBufferSize();
	vsByte.pShaderBytecode = vs->GetBufferPointer();

	D3D12_SHADER_BYTECODE psByte = {};
	psByte.BytecodeLength  = ps->GetBufferSize();
	psByte.pShaderBytecode = ps->GetBufferPointer();

	std::array<DXGI_FORMAT, GBUFFER_COUNT> formats = {
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* ALBEDO
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* NORMAL
		DXGI_FORMAT_R8G8B8A8_UNORM,		 //* MATERIAL
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //* VERTEX COLOR
		DXGI_FORMAT_R16G16B16A16_SNORM,  //* VERTEX NORMAL
		DXGI_FORMAT_R32G32B32A32_FLOAT,  //* WORLD POSITION
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	for (UINT i = 0; i < GBUFFER_COUNT; i++)
		psoDesc.RTVFormats[i] = formats[i];

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.NumRenderTargets = 6;
	psoDesc.pRootSignature = m_rootSignatures[ROOTSIGNATURE_STATE_GBUFFER].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = psByte;
	psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[INPUT_LAYOUT_INSTANCE_GBUFFER].data();
	psoDesc.InputLayout.NumElements = m_inputLayouts[INPUT_LAYOUT_INSTANCE_GBUFFER].size();
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSObjects[PIPELINE_STATE_GBUFFER]));
	CHECK_HR(result);

	m_PSObjects[PIPELINE_STATE_GBUFFER]->SetName(L"GBuffer PSO");
}

void PipelineStateHandler::InitializeSamplerDescs()
{
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;

	m_samplerDescs[SAMPLER_DESC_CLAMP] = samplerDesc;

	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1.f;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;

	m_samplerDescs[SAMPLER_DESC_WRAP] = samplerDesc;

	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1.f;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;

	m_samplerDescs[SAMPLER_DESC_BORDER] = samplerDesc;
}
void PipelineStateHandler::InitializeInputLayouts()
{
	m_inputLayouts[INPUT_LAYOUT_INSTANCE_SPRITE_2D] = {
		// Per Vertex
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV",		    0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		// Per Instance
		{ "SHEET_SIZE",     0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "FRAME_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "FRAME_ANCHOR",   0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "FRAME_SIZE",     0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "UI_SIZE",        0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	m_inputLayouts[INPUT_LAYOUT_INSTANCE_FONT_2D] = {
		// Per Instance

		{ "POSITION",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "FONT_SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "SHEET_SIZE",	   0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "NEW_POSITION",  0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "SIZE",          0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "ANCHOR",        0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "COLOR",         0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	m_inputLayouts[INPUT_LAYOUT_INSTANCE_MODEL] = {
		// Per Vertex
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0 },

		// Per Instance
		{ "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	m_inputLayouts[INPUT_LAYOUT_INSTANCE_GBUFFER] = {
		// Per Vertex
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		// Per Instance
		{ "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	//Used for debugging Collision
	m_inputLayouts[INPUT_LAYOUT_INSTANCE_AABB] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "SIZE",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

	m_inputLayouts[INPUT_LAYOUT_INSTANCE_RAY] = {
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "DESTINATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		//{ "DISTANCE",  0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};
}
