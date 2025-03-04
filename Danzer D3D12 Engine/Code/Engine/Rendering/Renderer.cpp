#include "stdafx.h"
#include "Renderer.h"

#include "Components/AllComponents.h"

#include "Camera.h"
#include "SkyBox.h"
#include "PSOHandler.h"
#include "Models/ModelHandler.h"
#include "RenderUtility.h"
#include "Rendering/Screen Rendering/Textures/FullscreenTexture.h"
#include "Rendering/Screen Rendering/DirectionalShadowMapping.h"
#include "Rendering/Screen Rendering/GBuffer.h"
#include "Core/FrameResource.h"

#include <chrono>
#include <string>

Renderer::~Renderer()
{
	m_framework		= nullptr;
	m_rootSignature = nullptr;
}

void Renderer::Init(D3D12Framework& framework)
{
	m_framework = &framework;
	
	m_shadowBuffer.Init(framework.GetDevice(),	   &framework.CbvSrvHeap(),	sizeof(CameraBuffer::Data));
	m_cameraBuffer.Init(framework.GetDevice(),	   &framework.CbvSrvHeap(),	sizeof(CameraBuffer::Data));
	m_lightBuffer.Init(framework.GetDevice(),	   &framework.CbvSrvHeap(),	sizeof(LightBuffer::Data));
	m_materialBuffer.Init(framework.GetDevice(),   &framework.CbvSrvHeap(),	sizeof(MaterialBuffer::Data));
	m_pointLightBuffer.Init(framework.GetDevice(), &framework.CbvSrvHeap(), sizeof(PointLightBuffer::Data));
}

//* Default Buffers for all existing 3D models. Should only be updated once per frame!
CD3DX12_GPU_DESCRIPTOR_HANDLE Renderer::UpdateDefaultBuffers(Camera& camera, Transform& transform, UINT frameIndex)
{
	// Reset descriptor index! 
	m_descriptorIndex = 0;
	
	CameraBuffer::Data bufferData;
	bufferData.m_transform  = transform.World().Invert();
	bufferData.m_transform  = DirectX::XMMatrixTranspose(bufferData.m_transform);
	bufferData.m_projection = DirectX::XMMatrixTranspose(camera.GetProjection());
	bufferData.m_position = { transform.m_position.x, transform.m_position.y, transform.m_position.z, float(camera.m_renderTarget)};
	Vect4f eye = { -bufferData.m_transform.Forward() };
	eye.w = 1.f;
	bufferData.m_direction = eye;
	bufferData.m_time = clock() / 1000.0f;
	bufferData.m_width  = static_cast<float>(WindowHandler::WindowData().m_w);
	bufferData.m_height = static_cast<float>(WindowHandler::WindowData().m_h);
	
	m_cameraBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&bufferData), frameIndex);

	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_cameraBuffer.OffsetID() + frameIndex, m_framework->CbvSrvHeap().DESCRIPTOR_SIZE());
	return cbvHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Renderer::UpdateShadowMapBuffer(const Mat4f& projection, const Mat4f& transform, const Vect4f& position, UINT frameIndex)
{
	CameraBuffer::Data bufferData;
	bufferData.m_transform  = transform.Invert();
	bufferData.m_transform  = DirectX::XMMatrixTranspose(bufferData.m_transform);
	bufferData.m_projection = DirectX::XMMatrixTranspose(projection);
	bufferData.m_position   = position;
	Vect4f eye = { transform.Forward()};
	eye.w = position.w;
	bufferData.m_direction = eye;

	m_shadowBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&bufferData), frameIndex);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle = m_framework->CbvSrvHeap().GET_GPU_DESCRIPTOR(0);
	cbvHandle.Offset((m_shadowBuffer.OffsetID() + frameIndex) * m_framework->CbvSrvHeap().DESCRIPTOR_SIZE());
	return cbvHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Renderer::UpdateLightBuffer(Mat4f& projection, Transform& transform, const DirectionalLight& light, const Vect4f& direction, UINT frameIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();

	LightBuffer::Data lightData;
	lightData.m_projection     = DirectX::XMMatrixTranspose(projection);
	lightData.m_transform      = transform.World().Invert();
	lightData.m_transform	   = DirectX::XMMatrixTranspose(lightData.m_transform);
	lightData.m_ambientColor   = light.m_ambientColor;
	lightData.m_lightColor     = light.m_lightColor;
	lightData.m_lightDirection = direction;

	m_lightBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&lightData), frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_lightBuffer.OffsetID() + frameIndex, cbvSrvDescSize);
	return cbvHandle;
}
void Renderer::RenderForwardModelEffects(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler, const UINT depthOffset, std::vector<ModelEffectData>& modelEffects, ModelHandler& modelHandler, std::vector<Texture>& textures, const UINT frameIndex, Camera& cam, Transform& camTransform, UINT startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();

	for (UINT i = 0; i < modelEffects.size(); i++)
	{
		ModelEffectData& effectData = modelEffects[i];
		
		// Set the effects RootSignature and Pipelinestate 
		cmdList->SetGraphicsRootSignature(psoHandler.GetRootSignature(effectData.GetRootsSignature()));
		cmdList->SetPipelineState(psoHandler.GetPipelineState(effectData.GetPSO()));

		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle = UpdateDefaultBuffers(cam, camTransform, frameIndex);
		cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

		CD3DX12_GPU_DESCRIPTOR_HANDLE depthHandle(cbvSrvHeapStart, depthOffset, cbvSrvDescSize);

		ModelData& model = modelHandler.GetLoadedModelInformation(effectData.ModelID());
		//model.UpdateTransformInstanceBuffer(effectData.GetTransforms(), frameIndex);

		for (UINT i = 0; i < model.GetMeshes().size(); i++)
		{
			ModelData::Mesh& mesh = model.GetSingleMesh(i);

			if (mesh.m_renderMesh) {

				int slot = 1;

				if (effectData.HasBuffer()) {	
					effectData.UpdateData(frameIndex);
					CD3DX12_GPU_DESCRIPTOR_HANDLE effectHandle(cbvSrvHeapStart, effectData.GetBuffer().OffsetID(), cbvSrvDescSize);
					cmdList->SetGraphicsRootDescriptorTable(slot, effectHandle);
					slot++;
				}

				std::vector<CD3DX12_GPU_DESCRIPTOR_HANDLE> srvHandles;
				srvHandles.emplace_back(depthHandle);

				for (UINT i = 0; i < effectData.GetTextures().size(); i++)
					srvHandles.emplace_back(cbvSrvHeapStart, textures[effectData.GetTextures()[i]].m_offsetID, cbvSrvDescSize);

				for (UINT i = 0; i < srvHandles.size(); i++)
					cmdList->SetGraphicsRootDescriptorTable(slot + i, srvHandles[i]);

				D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
							mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
				};
				cmdList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
				cmdList->IASetIndexBuffer(&mesh.m_indexBufferView);
				cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				
				for (UINT i = 0; i < effectData.GetTransforms().size(); i++)
					cmdList->DrawIndexedInstanced(mesh.m_numIndices, 1, 0, 0, i);	
			}
		}
		effectData.GetTransforms().clear();
	}
}
void Renderer::RenderToGbuffer(ID3D12GraphicsCommandList* cmdList, std::vector<ModelData>& models, UINT frameIndex, std::vector<Texture>& textures, bool renderTransparency, UINT startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const uint32_t cbvSrvDescSize			    = m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();

	for (uint32_t i = 0; i < models.size(); i++)
	{
		// Transparent models will not be rendered here.
		if (models[i].IsTransparent() == renderTransparency) {

			ModelData& model = models[i];

			if (!model.MeshToRender().empty()) {
				model.UpdateAllMeshInstanceBuffer(frameIndex);

				for (UINT j = 0; j < model.GetMeshes().size(); j++)
				{
					// Fetch mesh index from the meshes list that we will render.
					ModelData::Mesh& mesh = model.GetMeshes()[j];

					// Check if we want to render this mesh or if there are any instance transforms for this frame. 
					if (mesh.m_renderMesh && !mesh.m_instanceTransforms.empty()) {
						// Set Material buffer 

						MaterialBuffer::Data materialData;
						materialData.m_emissive  = mesh.m_material.m_emissvie;
						materialData.m_roughness = mesh.m_material.m_roughness;
						materialData.m_metallic  = mesh.m_material.m_shininess;
						for (UINT i = 0; i < 4; i++)
							materialData.m_color[i] = mesh.m_material.m_color[i];
						
						m_materialBuffer.UpdateBuffer(reinterpret_cast<UINT16*>(&materialData), frameIndex);
						CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_materialBuffer.OffsetID() + frameIndex, cbvSrvDescSize);
						cmdList->SetGraphicsRootDescriptorTable(1, cbvHandle);

						// Handle for each texture that will be used
						CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandles[6] = {
							CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, textures[mesh.m_material.m_albedo].m_offsetID,       cbvSrvDescSize), // Albedo
							CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, textures[mesh.m_material.m_normal].m_offsetID,	    cbvSrvDescSize), // Normal
							CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, textures[mesh.m_material.m_metallicMap].m_offsetID,  cbvSrvDescSize), // Metallic
							CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, textures[mesh.m_material.m_roughnessMap].m_offsetID, cbvSrvDescSize), // Roughness/Smoothness
							CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, textures[mesh.m_material.m_heightMap].m_offsetID,    cbvSrvDescSize), // Height 
							CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeapStart, textures[mesh.m_material.m_aoMap].m_offsetID,	    cbvSrvDescSize)  // AO
						};

						for (UINT i = 0; i < _countof(srvHandles); i++)
							cmdList->SetGraphicsRootDescriptorTable(2 + i, srvHandles[i]);

						// Set the vertex & transform buffers 
						D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
							mesh.m_vertexBufferView, mesh.m_meshBuffer.GetBufferView(frameIndex)
						};
						cmdList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
						cmdList->IASetIndexBuffer(&mesh.m_indexBufferView);
						cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						// Draw
						cmdList->DrawIndexedInstanced(mesh.m_numIndices, mesh.m_instanceTransforms.size(), 0, 0, 0);
					}
				}
			}
		}
	}
}
//void Renderer::TransparentRender(Scene* scene, std::vector<ModelData>& models, UINT frameIndex, std::vector<Texture> textures)
//{
//	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
//	const UINT cbvSrvDescSize = m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();
//
//	for (UINT i = 0; i < models.size(); i++)
//	{
//		if (models[i].IsTransparent()) {
//			ModelData& model = models[i];
//			UINT numberOfTransforms = ((UINT)model.GetInstanceTransforms().size() < MAX_INSTANCES_PER_MODEL) ? (UINT)model.GetInstanceTransforms().size() : MAX_INSTANCES_PER_MODEL;
//			
//			if (numberOfTransforms > 0) {
//				model.UpdateTransformInstanceBuffer(frameIndex);
//			}
//		}
//	}
//	//for (UINT i = 0; i < objects.size(); i++)
//	//{
//	//	if (models[objects[i].GetModel() - 1].IsTransparent()) {
//	//		ModelData& model = models[objects[i]->GetModel() - 1];
//	//
//	//		model.AddInstanceTransform(objects[i]->GetTransform() * scene->GetLayer(objects[i]->GetLayer()).m_layerTransform);
//	//		model.UpdateTransformInstanceBuffer(frameIndex);
//	//		
//	//		for (ModelData::Mesh& mesh : model.GetMeshes())
//	//		{
//	//			if (mesh.m_renderMesh) {
//	//				//Before rendering save current Descriptor index
//	//				UINT oldDescripterIndex = m_descriptorIndex;
//	//		
//	//				ID3D12DescriptorHeap* descHeaps[] = { textures[mesh.m_texture - 1].m_textureDescriptorHeap.Get() };
//	//				m_commandList->SetDescriptorHeaps(1, &descHeaps[0]);
//	//				m_commandList->SetGraphicsRootDescriptorTable(2, descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
//	//
//	//				D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
//	//					mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
//	//				};
//	//
//	//				m_commandList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
//	//				m_commandList->IASetIndexBuffer(&mesh.m_indexBufferView);
//	//				m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	//		
//	//				m_commandList->Draw(mesh.m_numIndices, 1, 0, 0);
//	//			}
//	//		}
//	//	}
//	//}
//}
//void Renderer::RayRendering(std::vector <RayBuffer::RayInstance > & rays, UINT frameIndex)
//{
//	m_rayBuffer.UpdateBuffer(reinterpret_cast<UINT8*>(&rays[0]), rays.size(), frameIndex);
//
//	m_commandList->IASetVertexBuffers(0, 1, &m_rayBuffer.GetBufferView(frameIndex));
//	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
//
//	m_commandList->DrawInstanced(1, (UINT)rays.size(), 0, 0);
//}

