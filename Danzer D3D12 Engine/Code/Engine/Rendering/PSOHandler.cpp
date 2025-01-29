#include "stdafx.h"
#include "PSOHandler.h"

#include "Core/D3D12Framework.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

PSOHandler::PSOHandler(D3D12Framework& framework) :
	m_device(framework.GetDevice())
{
	InitializeSamplerDescs();
	InitializeInputLayouts();
	InitializeRastDescs();
	InitializeBlendDescs();
}

const uint32_t PSOHandler::CreateRootSignature(const UINT numberOfCBV, const UINT numberOfSRV, SAMPLER_DESCS sampler, D3D12_ROOT_SIGNATURE_FLAGS flags, std::wstring name)
{
	std::vector<CD3DX12_ROOT_PARAMETER> rootParameter = {};
	rootParameter.reserve(size_t(numberOfCBV) + size_t(numberOfSRV));

	std::vector<CD3DX12_DESCRIPTOR_RANGE> cbvRanges = {};
	cbvRanges.reserve(numberOfCBV);
	for (UINT i = 0; i < numberOfCBV; i++)
	{
		cbvRanges.emplace_back(CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, i));
		rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
		rootParameter[i].InitAsDescriptorTable(1, &cbvRanges[i]);
	}

	std::vector<CD3DX12_DESCRIPTOR_RANGE> srvRanges = {};
	srvRanges.reserve(numberOfSRV);
	for (UINT i = 0; i < numberOfSRV; i++)
	{
		srvRanges.emplace_back(CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i));
		rootParameter.emplace_back(CD3DX12_ROOT_PARAMETER());
		rootParameter[i + numberOfCBV].InitAsDescriptorTable(1, &srvRanges[i]);
	}
	
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init((UINT)rootParameter.size(), &rootParameter[0], 1, &m_samplerDescs[sampler], flags);

	ID3DBlob* signature = nullptr;
	ID3DBlob* error		= nullptr;

	m_rootSignatures.emplace_back(ComPtr<ID3D12RootSignature>());
	UINT index = (UINT)m_rootSignatures.size() - 1;

	CHECK_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	CHECK_HR(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[index])));
	
	m_rootSignatures[index]->SetName(name.c_str());

	return index;
}

const uint32_t PSOHandler::CreateDefaultPSO(std::wstring vertexShader, std::wstring pixelShader, BLEND_DESC blend, RASTERIZER_DESC rast, D3D12_DEPTH_STENCIL_DESC depth, DXGI_FORMAT depthFormat, DXGI_FORMAT* rtvFormats, const UINT rtvCount, const UINT rootSignature, INPUT_LAYOUTS layout, std::wstring name)
{
	DXGI_SAMPLE_DESC sample = { 1, 0 };

	D3D12_SHADER_BYTECODE vsByte, psByte;
	ReadFileToBlob(vertexShader, &vsByte);
	ReadFileToBlob(pixelShader,  &psByte);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	if (rtvFormats != nullptr)
		for (UINT i = 0; i < rtvCount; i++)
			psoDesc.RTVFormats[i] = rtvFormats[i];
	else
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

	psoDesc.DSVFormat			  = depthFormat;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets	  = rtvFormats ? rtvCount : 0;
	psoDesc.SampleDesc			  = sample;
	psoDesc.SampleMask			  = 0xffffffff;
	psoDesc.BlendState			  = m_blendDescs[blend];
	psoDesc.RasterizerState		  = m_rastDescs[rast];
	psoDesc.DepthStencilState	  = depth;
	psoDesc.pRootSignature		  = m_rootSignatures[rootSignature].Get();
	psoDesc.VS					  = vsByte;
	psoDesc.PS				      = pixelShader.size() > 0 ? psByte : CD3DX12_SHADER_BYTECODE(0, 0);
	psoDesc.Flags				  = D3D12_PIPELINE_STATE_FLAG_NONE;
	if (layout != IL_NONE) {
		psoDesc.InputLayout.pInputElementDescs = m_inputLayouts[layout].data();
		psoDesc.InputLayout.NumElements        = (UINT)m_inputLayouts[layout].size();
	}

	m_pipelineStates.emplace_back(ComPtr<ID3D12PipelineState>());
	UINT index = (UINT)m_pipelineStates.size() - 1;
	CHECK_HR(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStates[index])));
	
	m_pipelineStates[index]->SetName(name.c_str());

	return index;
}

const uint32_t PSOHandler::CreateDefaultPSO(std::wstring vertexShader, std::wstring pixelShader, BLEND_DESC blend, RASTERIZER_DESC rast, D3D12_DEPTH_STENCIL_DESC depth, DXGI_FORMAT depthFormat, DXGI_FORMAT* rtvFormats, const UINT rtvCount, const UINT rootSignature, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, std::wstring name)
{
	DXGI_SAMPLE_DESC sample = { 1, 0 };

	D3D12_SHADER_BYTECODE vsByte, psByte;
	ReadFileToBlob(vertexShader, &vsByte);
	ReadFileToBlob(pixelShader,  &psByte);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	
	if (rtvFormats != nullptr)
		for (UINT i = 0; i < rtvCount; i++)
			psoDesc.RTVFormats[i] = rtvFormats[i];
	else
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

	psoDesc.DSVFormat = depthFormat;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = rtvFormats ? rtvCount : 0;
	psoDesc.SampleDesc = sample;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.BlendState = m_blendDescs[blend];
	psoDesc.RasterizerState = m_rastDescs[rast];
	psoDesc.DepthStencilState = depth;
	psoDesc.pRootSignature = m_rootSignatures[rootSignature].Get();
	psoDesc.VS = vsByte;
	psoDesc.PS = pixelShader.size() > 0 ? psByte : CD3DX12_SHADER_BYTECODE(0, 0);
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.InputLayout.pInputElementDescs = layout.data();
	psoDesc.InputLayout.NumElements		   = (uint32_t)layout.size();	

	m_pipelineStates.emplace_back(ComPtr<ID3D12PipelineState>());
	UINT index = (UINT)m_pipelineStates.size() - 1;
	CHECK_HR(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStates[index])));

	m_pipelineStates[index]->SetName(name.c_str());

	return index;
}

const uint32_t PSOHandler::CreateGeometryPSO(std::wstring vertexShader, std::wstring geometryShader, std::wstring pixelShader, BLEND_DESC blend, RASTERIZER_DESC rast, D3D12_DEPTH_STENCIL_DESC depth, DXGI_FORMAT depthFormat, DXGI_FORMAT* rtvFormats, const UINT rtvCount, const UINT rootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, std::wstring name)
{
	DXGI_SAMPLE_DESC sample = { 1, 0 };

	D3D12_SHADER_BYTECODE vsByte, gsByte, psByte;
	ReadFileToBlob(vertexShader,   &vsByte);
	ReadFileToBlob(geometryShader, &gsByte);
	ReadFileToBlob(pixelShader,	   &psByte);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	if (rtvFormats != nullptr)
		for (UINT i = 0; i < rtvCount; i++)
			psoDesc.RTVFormats[i] = rtvFormats[i];
	else
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

	psoDesc.DSVFormat					   = depthFormat;
	psoDesc.PrimitiveTopologyType		   = primitiveTopology;
	psoDesc.NumRenderTargets			   = rtvFormats ? rtvCount : 0;
	psoDesc.SampleDesc					   = sample;
	psoDesc.SampleMask					   = 0xffffffff;
	psoDesc.BlendState					   = m_blendDescs[blend];
	psoDesc.RasterizerState				   = m_rastDescs[rast];
	psoDesc.DepthStencilState			   = depth;
	psoDesc.pRootSignature				   = m_rootSignatures[rootSignature].Get();
	psoDesc.VS							   = vsByte;
	psoDesc.GS							   = gsByte;
	psoDesc.PS							   = psByte;
	psoDesc.Flags						   = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.InputLayout.pInputElementDescs = layout.data();
	psoDesc.InputLayout.NumElements		   = (uint32_t)layout.size();

	m_pipelineStates.emplace_back(ComPtr<ID3D12PipelineState>());
	UINT index = (UINT)m_pipelineStates.size() - 1;
	CHECK_HR(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStates[index])));

	m_pipelineStates[index]->SetName(name.c_str());

	return index;
}

void PSOHandler::InitializeSamplerDescs()
{
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;

	m_samplerDescs[SAMPLER_CLAMP] = samplerDesc;

	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;

	m_samplerDescs[SAMPLER_WRAP] = samplerDesc;

	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;
	//samplerDesc
	
	m_samplerDescs[SAMPLER_BORDER] = samplerDesc;
}

void PSOHandler::InitializeInputLayouts()
{
	m_inputLayouts[IL_INSTANCE_SPRITE_2D] = {
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

	m_inputLayouts[IL_INSTANCE_FONT_2D] = {
		// Per Instance
		{ "POSITION",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "FONT_SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "SHEET_SIZE",	   0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "NEW_POSITION",  0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "SIZE",          0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "ANCHOR",        0, DXGI_FORMAT_R32G32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "COLOR",         0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	m_inputLayouts[IL_INSTANCE_DEFFERED] = {
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

	m_inputLayouts[IL_INSTANCE_FORWARD] = {
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
}

void PSOHandler::InitializeBlendDescs()
{
	m_blendDescs[BLEND_DEFAULT] = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_blendDescs[BLEND_DEFAULT].RenderTarget[0].BlendEnable = false;

	m_blendDescs[BLEND_TRANSPARENT] = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].BlendEnable = true;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_MAX;
	m_blendDescs[BLEND_TRANSPARENT].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	m_blendDescs[BLEND_TRANSPARENT].AlphaToCoverageEnable = false;
}

void PSOHandler::InitializeRastDescs()
{
	CD3DX12_RASTERIZER_DESC rast(
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_FRONT, FALSE,
		D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, TRUE, FALSE,
		0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	);

	m_rastDescs[RASTERIZER_FRONT] = rast;

	rast.CullMode = D3D12_CULL_MODE_BACK;
	m_rastDescs[RASTERIZER_BACK]  = rast;

	rast.CullMode = D3D12_CULL_MODE_NONE;
	m_rastDescs[RASTERIZER_NONE]  = rast;

	rast = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_rastDescs[RASTERIZER_DEFAULT] = rast;
	
	rast.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rast.CullMode = D3D12_CULL_MODE_NONE;
	m_rastDescs[RASTERIZER_LINE] = rast;
}

void PSOHandler::ReadFileToBlob(const std::wstring& shader, D3D12_SHADER_BYTECODE* byteCode)
{
	if (shader.empty())
		return;

	ID3DBlob* blob = nullptr;
	CHECK_HR(D3DReadFileToBlob(shader.c_str(), &blob));
	byteCode->BytecodeLength  = blob->GetBufferSize();
	byteCode->pShaderBytecode = blob->GetBufferPointer();
}

