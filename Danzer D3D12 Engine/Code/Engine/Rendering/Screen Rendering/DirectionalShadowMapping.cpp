#include "stdafx.h"
#include "DirectionalShadowMapping.h"
#include "Rendering/PSOHandler.h"

#include "Core/DesriptorHeapWrapper.h"

void DirectionalShadowMapping::RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& dsvHeap, DescriptorHeapWrapper& cbvSrvheap, const uint8_t frameIndex)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap.GET_CPU_DESCRIPTOR(m_dsvHeapIndex + frameIndex);

	cmdList->RSSetViewports(1, &m_viewPort);
	cmdList->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

	for (UINT i = 0; i < modelCount; i++)
	{
		if (!m_models[i].IsTransparent()) {
			ModelData& model = m_models[i];
			UINT numberOfTransforms = ((UINT)model.GetInstanceTransforms().size() < MAX_INSTANCES_PER_MODEL) ? (UINT)model.GetInstanceTransforms().size() : MAX_INSTANCES_PER_MODEL;

			if (numberOfTransforms > 0) {
				for (UINT j = 0; j < model.GetMeshes().size(); j++)
				{
					ModelData::Mesh& mesh = model.GetMeshes()[j];
					if (mesh.m_renderMesh) {
						D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
							mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
						};
						cmdList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
						cmdList->IASetIndexBuffer(&mesh.m_indexBufferView);
						cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
						cmdList->DrawIndexedInstanced(mesh.m_numIndices, numberOfTransforms, 0, 0, 0);
					}
				}
			}
		}
	}
}

void DirectionalShadowMapping::SetPipelineAndRootSignature(PSOHandler& psoHandler)
{
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = true;
	depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depth.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depth.StencilEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS     |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS   |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	InitializeRenderer(
		L"Shaders/ShadowDepthVS.cso",
		L"",
		8192,
		8192,
		true,
		depth,
		DXGI_FORMAT_D32_FLOAT,
		std::vector<DXGI_FORMAT>(),
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_NONE,
		PSOHandler::SAMPLER_BORDER,
		flags,
		1,
		0,
		PSOHandler::IL_INSTANCE_DEFFERED,
		L"Directional Shadow Mapping",
		psoHandler,
		true
	);
}

void DirectionalShadowMapping::CreateProjection(float projectionScale, float increase)
{
	m_projectionMatrix = DirectX::XMMatrixOrthographicLH(projectionScale, projectionScale, -(projectionScale * increase), projectionScale * increase);
	//m_projectionMatrix = DirectX::XMMatrixOrthographicLH(projectionScale, projectionScale, -10.0f, 10.0f);
}