#include "stdafx.h"
#include "Renderer.h"

#include "Scene.h"
#include "Components/Transform.h";
#include "Components/Object.h"
#include "Components/DirectionalLight.h"
#include "SkyBox.h"
#include "Rendering/Screen Rendering/GBuffer.h"
#include "Screen Rendering/DirectionalShadowMapping.h"

#include "Camera.h"

#include "RenderUtility.h"
#include "Core/FrameResource.h"


#include "Core/D3D12Framework.h"

#include <string>

Renderer::~Renderer()
{
	//m_cameraBuffer.~CameraBuffer();
	//m_transformBuffer.~TransformBuffer();

	//m_commandList = nullptr;
	m_rootSignature = nullptr;
}

void Renderer::Init(D3D12Framework& framework)
{
	m_framework = &framework;
	//m_commandList = framework.InitCmdList();
	
	m_shadowBuffer.Init(framework.GetDevice(),   &framework.CbvSrvHeap(), m_shadowBuffer.FetchData(),   sizeof(CameraBuffer::Data));
	m_cameraBuffer.Init(framework.GetDevice(),   &framework.CbvSrvHeap(), m_cameraBuffer.FetchData(),   sizeof(CameraBuffer::Data));
	m_lightBuffer.Init(framework.GetDevice(),    &framework.CbvSrvHeap(), m_lightBuffer.FetchData(),    sizeof(LightBuffer::Data));
	m_materialBuffer.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_materialBuffer.FetchData(), sizeof(MaterialBuffer::Data));

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
	Vect4f eye = { bufferData.m_transform.Forward() };
	eye.w = 1.f;
	bufferData.m_direction = eye;

	m_cameraBuffer.UpdateBuffer(&bufferData, frameIndex);

	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_cameraBuffer.OffsetID() + frameIndex, m_framework->CbvSrvHeap().DESCRIPTOR_SIZE());
	return cbvHandle;
	//m_commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Renderer::UpdateShadowMapBuffer(Mat4f& projection, Transform& transform, UINT frameIndex)
{
	CameraBuffer::Data bufferData;
	bufferData.m_transform = transform.World().Invert();
	bufferData.m_transform = DirectX::XMMatrixTranspose(bufferData.m_transform);
	bufferData.m_projection = DirectX::XMMatrixTranspose(projection);
	bufferData.m_position = { transform.m_position.x, transform.m_position.y, transform.m_position.z, 0.0f };
	Vect4f eye = { bufferData.m_transform.Forward() };
	eye.w = 1.f;
	bufferData.m_direction = eye;

	m_shadowBuffer.UpdateBuffer(&bufferData, frameIndex);

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

	m_lightBuffer.UpdateBuffer(&lightData, frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_lightBuffer.OffsetID() + frameIndex, cbvSrvDescSize);
	return cbvHandle;
}

void Renderer::RenderSkybox(ID3D12GraphicsCommandList* cmdList, Transform& cameraTransform, TextureHandler::Texture& textures, ModelData& model, Skybox& skybox, UINT frameIndex, UINT startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize					= m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();
	
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, textures.m_offsetID, cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(1, srvHandle);
	
	ModelData::Mesh& mesh = model.GetMeshes()[0];
	
	if (!model.GetInstanceTransforms().empty())
		 model.ClearInstanceTransform();
	
	//* Add the rotation from the skybox and always place it a the cameras position
	DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&skybox.GetRotation());
	Mat4f transform = DirectX::XMMatrixRotationQuaternion(quatv);
	transform      *= DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f);
	transform      *= DirectX::XMMatrixTranslation(cameraTransform.m_position.x, cameraTransform.m_position.y, cameraTransform.m_position.z);

	model.AddInstanceTransform(DirectX::XMMatrixTranspose(transform));
	model.UpdateTransformInstanceBuffer(frameIndex);
	D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
		mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
	};
	cmdList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
	cmdList->IASetIndexBuffer(&mesh.m_indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	cmdList->DrawIndexedInstanced(mesh.m_numIndices, 1, 0, 0, 0);
}

void Renderer::RenderDirectionalLight(ID3D12GraphicsCommandList* cmdList, TextureHandler::Texture& skyboxTexture, DirectionalShadowMapping& shadowMap, UINT frameIndex, UINT& startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, skyboxTexture.m_offsetID, cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(startLocation, srvHandle);
	startLocation++;

	CD3DX12_GPU_DESCRIPTOR_HANDLE shadowHandle(cbvSrvHeapStart, shadowMap.SRVOffsetID(), cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(startLocation, shadowHandle);

	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	cmdList->DrawInstanced(3, 1, 0, 0);
}

void Renderer::RenderToGbuffer(ID3D12GraphicsCommandList* cmdList, std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures, bool renderTransparency, UINT startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize					= m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();

	for (UINT i = 0; i < models.size(); i++)
	{
		if (models[i].IsTransparent() == renderTransparency) {

			ModelData& model = models[i];
			UINT numberOfTransforms = ((UINT)model.GetInstanceTransforms().size() < MAX_INSTANCES_PER_MODEL) ? (UINT)model.GetInstanceTransforms().size() : MAX_INSTANCES_PER_MODEL;

			if (numberOfTransforms > 0) {
				model.UpdateTransformInstanceBuffer(frameIndex);

				int id = 0;

				for (UINT j = 0; j < model.GetMeshes().size(); j++)
				{
					ModelData::Mesh& mesh = model.GetMeshes()[j];
					if (mesh.m_renderMesh) {

						MaterialBuffer::Data materialData;
						materialData.m_emissive = mesh.m_material.m_emissvie;
						materialData.m_roughness = mesh.m_material.m_roughness;
						materialData.m_metallic = mesh.m_material.m_shininess;
						for (UINT i = 0; i < 4; i++)
							materialData.m_color[i] = mesh.m_material.m_color[i];
						
						m_materialBuffer.UpdateBuffer(&materialData, frameIndex);
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
//void Renderer::TransparentRender(Scene* scene, std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture> textures)
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
//
//void Renderer::AABBRendering(std::vector<AABBBuffer::AABBInstance>& aabb, UINT frameIndex)
//{
//	m_aabbBuffer.UpdateBuffer(reinterpret_cast<UINT8*>(&aabb[0]), aabb.size(), frameIndex);
//
//	m_commandList->IASetVertexBuffers(0, 1, &m_aabbBuffer.GetBufferView(frameIndex));
//	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
//
//	m_commandList->DrawInstanced(1, (UINT)aabb.size(), 0, 0);
//}

