#include "stdafx.h"
#include "DirectionalShadowMapping.h"

#include "Core/DesriptorHeapWrapper.h"

void DirectionalShadowMapping::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& handle, const UINT frameIndex)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = handle.GET_CPU_DESCRIPTOR(0);
	dsvHandle.Offset((DSVOffsetID() + frameIndex) * handle.DESCRIPTOR_SIZE());

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

void DirectionalShadowMapping::CreateProjection(float projectionScale, float increase)
{
	m_projectionMatrix = DirectX::XMMatrixOrthographicLH(projectionScale, projectionScale, -(projectionScale * increase), projectionScale * increase);
}
