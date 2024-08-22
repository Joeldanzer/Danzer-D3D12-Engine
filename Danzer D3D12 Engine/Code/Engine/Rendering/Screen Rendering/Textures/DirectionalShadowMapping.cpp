#include "stdafx.h"
#include "DirectionalShadowMapping.h"
#include "Rendering/PSOHandler.h"

#include "Core/DesriptorHeapWrapper.h"

void DirectionalShadowMapping::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper* handle, TextureHandler* textureHandler, const UINT frameIndex)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = handle->GET_CPU_DESCRIPTOR(0);
	dsvHandle.Offset((DSVOffsetID() + frameIndex) * handle->DESCRIPTOR_SIZE());

	cmdList->RSSetViewports(1, &GetViewPort());
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
	m_rs  = psoHandler.CreateRootSignature(1, 0, PSOHandler::SAMPLER_DESC_BORDER, flags, L"Dirrectional Shadow Mapping Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/ShadowDepthVS.cso", L"" },
		CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		psoHandler.RastDescs(PSOHandler::RASTERIZER_NONE),
		depth,
		DXGI_FORMAT_D32_FLOAT,
		nullptr,
		0,
		m_rs,
		PSOHandler::INPUT_LAYOUT_INSTANCE_DEFFERED,
		L"Directional Shadow Mapping PSO"
	);
}

void DirectionalShadowMapping::CreateProjection(float projectionScale, float increase)
{
	m_projectionMatrix = DirectX::XMMatrixOrthographicLH(projectionScale, projectionScale, -(projectionScale * increase), projectionScale * increase);
	
	//m_projectionMatrix = DirectX::XMMatrixOrthographicLH(projectionScale, projectionScale, -10.0f, 10.0f);
}
