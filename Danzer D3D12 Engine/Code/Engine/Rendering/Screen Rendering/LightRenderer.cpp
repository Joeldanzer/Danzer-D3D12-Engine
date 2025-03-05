#include "stdafx.h"
#include "LightRenderer.h"

#include "Rendering/Data/LightHandler.h"
#include "Rendering/PSOHandler.h"

void LightRenderer::InitializeRenderer(PSOHandler& psoHandler, const uint32_t cbvCount, const uint32_t srvCount)
{
	D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	m_rs = psoHandler.CreateRootSignature(cbvCount, srvCount, PSOHandler::SAMPLER_WRAP, flags, L"Spot & Point Light Root Signature");
	
	std::vector<D3D12_INPUT_ELEMENT_DESC> pointLightDesc = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "RADIUS",   0, DXGI_FORMAT_R32_FLOAT,			 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	std::vector<D3D12_INPUT_ELEMENT_DESC> spotLightDesc = {
		{ "POSITION",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "COLOR",         0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "DIRECTION",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "CUT_OFF",       0, DXGI_FORMAT_R32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "OUTER_CUT_OFF", 0, DXGI_FORMAT_R32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "RANGE",         0, DXGI_FORMAT_R32_FLOAT,		  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	DXGI_FORMAT format[2] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pointLightPSO = psoHandler.GetPipelineState(psoHandler.CreateDefaultPSO(
		L"Shaders/PointLightVS.cso",
		L"Shaders/PointLightPS.cso",
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_DEFAULT,
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		2,
		m_rs,
		pointLightDesc,
		L"Point Light PSO"
	));

	m_spotLightPSO = psoHandler.GetPipelineState(psoHandler.CreateDefaultPSO(
		L"Shaders/SpotLightVS.cso",
		L"Shaders/SpotLightPS.cso",
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_DEFAULT,
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		2,
		m_rs,
		spotLightDesc,
		L"Spot Light PSO"
	));

	m_bufferSlots.reserve(cbvCount);
	for (uint32_t i = 0; i < cbvCount; i++)
		m_bufferSlots.emplace_back(UINT32_MAX, true);

	m_textureSlots.reserve(srvCount);
	for (uint32_t i = 0; i < srvCount; i++)
		m_textureSlots.emplace_back(UINT32_MAX, true);
	
	m_rtvSlots.reserve(3);
	for (uint32_t i = 0; i < 3; i++)
		m_rtvSlots.emplace_back(UINT32_MAX, true);
}

bool LightRenderer::RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& rtvHeap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex)
{
	m_lightHandler.UpdateLightInstances(frameIndex);
	
	// PointLights 
	{
		if (!m_lightHandler.m_pointLightInstances.empty()) {
			cmdList->SetPipelineState(m_pointLightPSO);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle[2] = {
				rtvHeap.GET_CPU_DESCRIPTOR(m_rtvSlots[0].first + frameIndex),
				rtvHeap.GET_CPU_DESCRIPTOR(m_rtvSlots[2].first + frameIndex)
			};

			cmdList->OMSetRenderTargets(2, &rtvHandle[0], false, nullptr);
			
			D3D12_VERTEX_BUFFER_VIEW vertexBuffer[1] = { m_lightHandler.m_pointLightBuffer.GetBufferView(frameIndex) };
			cmdList->IASetVertexBuffers(0, 1, &vertexBuffer[0]);
			cmdList->IASetIndexBuffer(nullptr);
			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			cmdList->DrawInstanced(3, (uint32_t)m_lightHandler.m_pointLightInstances.size(), 0, 0);
		}
	}


	// SpotLights
	{
		if (!m_lightHandler.m_spotLightInstances.empty()) {
			cmdList->SetPipelineState(m_spotLightPSO);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle[2] = {
				rtvHeap.GET_CPU_DESCRIPTOR(m_rtvSlots[1].first + frameIndex),
				rtvHeap.GET_CPU_DESCRIPTOR(m_rtvSlots[2].first + frameIndex)
			};

			cmdList->OMSetRenderTargets(2, &rtvHandle[0], false, nullptr);

			cmdList->IASetVertexBuffers(0, 1, &m_lightHandler.m_spotLightBuffer.GetBufferView(frameIndex));
			cmdList->IASetIndexBuffer(nullptr);
			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			cmdList->DrawInstanced(3, (uint32_t)m_lightHandler.m_spotLightInstances.size(), 0, 0);
		}
	}

	m_lightHandler.ClearAllInstances();
	
	return true;
}
