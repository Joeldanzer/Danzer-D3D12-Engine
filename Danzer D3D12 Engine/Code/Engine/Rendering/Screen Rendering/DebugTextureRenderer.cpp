#include "stdafx.h"
#include "DebugTextureRenderer.h"

#include "Rendering/Models/ModelHandler.h"
#include "Rendering/Data/DebugRenderingData.h"

#include "Rendering/PSOHandler.h"

DebugTextureRenderer::DebugTextureRenderer(ModelHandler& modelHandler) :
	m_cubeID(modelHandler.CreateCustomModel(ModelData::GetCube()).m_modelID),
	m_modelHandler(modelHandler),
    m_debugData(nullptr),
	m_spherePSO(nullptr),
	m_aabbPSO(nullptr),
	m_linePSO(nullptr)
{
}

DebugTextureRenderer::~DebugTextureRenderer(){}

void DebugTextureRenderer::InitializeDebugRenderer(const uint16_t viewportWidth, const uint16_t viewportHeight, std::vector<DXGI_FORMAT> formats, PSOHandler& psoHandler)
{
	m_rs = psoHandler.CreateRootSignature(
		1,
		0,
		PSOHandler::SAMPLER_CLAMP,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS     |
		D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
		L" Debug Renderer Root Signature"
	);

	std::vector<D3D12_INPUT_ELEMENT_DESC> aabbInputDesc = {
		// Per Vertex
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		// Per Instance
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	std::vector<D3D12_INPUT_ELEMENT_DESC> lineInputDesc = {
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "END",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;

	m_pso = psoHandler.CreateDefaultPSO(
		L"Shaders/AABBVS.cso", 
		L"Shaders/AABBPS.cso",
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_LINE,
		depth,
		DXGI_FORMAT_UNKNOWN,
		formats.empty() ? nullptr : &formats[0],
		formats.empty() ? 0 : (uint32_t)formats.size(),
		m_rs,
		aabbInputDesc,
		L" AABB Debug PSO"
	);
	m_aabbPSO = psoHandler.GetPipelineState(m_pso);

	m_pso  = psoHandler.CreateGeometryPSO(
		L"Shaders/DebugLineVS.cso", 
		L"Shaders/DebugLineGS.cso",
		L"Shaders/DebugLinePS.cso",
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_LINE,
		depth,
		DXGI_FORMAT_UNKNOWN,
		formats.empty() ? nullptr : &formats[0],
		formats.empty() ? 0 : (uint32_t)formats.size(),
		m_rs,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
		lineInputDesc,
		L"Line Debug PSO"
	);
	m_linePSO = psoHandler.GetPipelineState(m_pso);

	m_bufferSlots.push_back({ UINT32_MAX, true });
	m_rtvSlots.push_back({ UINT32_MAX, true });

	m_rendererName = L"Debug Texture Renderer";
	m_renderAsDepth = false;
	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(viewportWidth), FLOAT(viewportHeight));
}

bool DebugTextureRenderer::RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvHeap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex)
{
	cmdList->RSSetViewports(1, &m_viewPort);
	if (m_rtvSlots.empty())
		return false;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap.GET_CPU_DESCRIPTOR(m_rtvSlots[0].first + frameIndex);
	cmdList->OMSetRenderTargets(1, &rtvHandle, 0, nullptr);

	m_debugData->UpdateInstancesForRendering(frameIndex);
	RenderDebugAABBs(cmdList, frameIndex);
	RenderDebugLines(cmdList, frameIndex);

	m_debugData->ClearInstances(); // Clear all instances of debug shapes. 

	return true;
}

void DebugTextureRenderer::RenderDebugAABBs(ID3D12GraphicsCommandList* cmdList, const uint32_t frameIndex)
{
	if (m_debugData->m_aabbRenderList.empty())
		return;

	cmdList->SetPipelineState(m_aabbPSO);

	const ModelData::Mesh& mesh = m_modelHandler.GetLoadedModelInformation(m_cubeID).GetSingleMesh(0);

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer[2]{
		mesh.m_vertexBufferView, m_debugData->m_vertexAABBBuffer.GetBufferView(frameIndex)
	};

	cmdList->IASetVertexBuffers(0, 2, &vertexBuffer[0]);
	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetIndexBuffer(&mesh.m_indexBufferView);

	cmdList->DrawInstanced(mesh.m_numVerticies, (uint32_t)m_debugData->m_aabbRenderList.size(), 0, 0);
}

void DebugTextureRenderer::RenderDebugLines(ID3D12GraphicsCommandList* cmdList, const uint32_t frameIndex)
{
	if (m_debugData->m_lineRenderList.empty())
		return;

	cmdList->SetPipelineState(m_linePSO);

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = {
		m_debugData->m_vertexLineBuffer.GetBufferView(frameIndex)
	};
	cmdList->IASetVertexBuffers(0, 1, &vertexBuffer);
	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->DrawInstanced(1, (uint32_t)m_debugData->m_lineRenderList.size(), 0, 0);
}



