#include "stdafx.h"
#include "Renderer.h"

#include "Scene.h"
#include "Components/Transform.h";
#include "Components/Object.h"
#include "Components/DirectionalLight.h"
#include "SkyBox.h"
#include "Rendering/Screen Rendering/GBuffer.h"

#include "Camera.h"

#include "RenderUtility.h"


#include "Core/DirectX12Framework.h"

#include <string>

Renderer::~Renderer()
{
	//m_cameraBuffer.~CameraBuffer();
	//m_transformBuffer.~TransformBuffer();

	m_commandList = nullptr;
	m_rootSignature = nullptr;
}

void Renderer::Init(DirectX12Framework& framework)
{
	m_framework = &framework;
	m_commandList = framework.GetCommandList();
	
	m_cameraBuffer.Init(framework.GetDevice(),   &framework.GetCbvSrvUavWrapper(), m_cameraBuffer.FetchData(),   sizeof(CameraBuffer::Data));
	m_lightBuffer.Init(framework.GetDevice(),    &framework.GetCbvSrvUavWrapper(), m_lightBuffer.FetchData(),    sizeof(LightBuffer::Data));
	m_materialBuffer.Init(framework.GetDevice(), &framework.GetCbvSrvUavWrapper(), m_materialBuffer.FetchData(), sizeof(MaterialBuffer::Data));

}

//* Default Buffers for all existing 3D models. Should only be updated once per frame!
void Renderer::UpdateDefaultBuffers(Camera& camera, Transform& transform, UINT frameIndex)
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

	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->GetCbvSrvUavWrapper().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_cameraBuffer.OffsetID() + frameIndex, m_framework->GetCbvSrvUavWrapper().DESCRIPTOR_SIZE());
	m_commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
}

void Renderer::RenderSkybox(Transform& cameraTransform, TextureHandler::Texture& textures, ModelData& model, Skybox& skybox, UINT frameIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->GetCbvSrvUavWrapper().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize					= m_framework->GetCbvSrvUavWrapper().DESCRIPTOR_SIZE();
	
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, textures.m_offsetID, cbvSrvDescSize);
	m_commandList->SetGraphicsRootDescriptorTable(1, srvHandle);
	
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
	m_commandList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
	m_commandList->IASetIndexBuffer(&mesh.m_indexBufferView);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	m_commandList->DrawIndexedInstanced(mesh.m_numIndices, 1, 0, 0, 0);
}

void Renderer::RenderDirectionalLight(TextureHandler::Texture& skyboxTexture, UINT frameIndex, UINT& startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->GetCbvSrvUavWrapper().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = m_framework->GetCbvSrvUavWrapper().DESCRIPTOR_SIZE();

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, skyboxTexture.m_offsetID, cbvSrvDescSize);
	m_commandList->SetGraphicsRootDescriptorTable(startLocation, srvHandle);

	m_commandList->IASetVertexBuffers(0, 0, nullptr);
	m_commandList->IASetIndexBuffer(nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	m_commandList->DrawInstanced(3, 1, 0, 0);
}

void Renderer::RenderToGbuffer(std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures, bool renderTransparency)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->GetCbvSrvUavWrapper().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize					= m_framework->GetCbvSrvUavWrapper().DESCRIPTOR_SIZE();

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
						m_commandList->SetGraphicsRootDescriptorTable(1, cbvHandle);
						
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
							m_commandList->SetGraphicsRootDescriptorTable(i+2, srvHandles[i]);

						D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
							mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
						};
						m_commandList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
						m_commandList->IASetIndexBuffer(&mesh.m_indexBufferView);
						m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						m_commandList->DrawIndexedInstanced(mesh.m_numIndices, numberOfTransforms, 0, 0, 0);
					}
				}
			}
		}
	}
}
//void Renderer::TransparentRender(Scene* scene, std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture> textures)
//{
//	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->GetCbvSrvUavWrapper().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
//	const UINT cbvSrvDescSize = m_framework->GetCbvSrvUavWrapper().DESCRIPTOR_SIZE();
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
void Renderer::RayRendering(std::vector <RayBuffer::RayInstance > & rays, UINT frameIndex)
{
	m_rayBuffer.UpdateBuffer(reinterpret_cast<UINT8*>(&rays[0]), rays.size(), frameIndex);

	m_commandList->IASetVertexBuffers(0, 1, &m_rayBuffer.GetBufferView(frameIndex));
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	m_commandList->DrawInstanced(1, (UINT)rays.size(), 0, 0);
}

void Renderer::AABBRendering(std::vector<AABBBuffer::AABBInstance>& aabb, UINT frameIndex)
{
	m_aabbBuffer.UpdateBuffer(reinterpret_cast<UINT8*>(&aabb[0]), aabb.size(), frameIndex);

	m_commandList->IASetVertexBuffers(0, 1, &m_aabbBuffer.GetBufferView(frameIndex));
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	m_commandList->DrawInstanced(1, (UINT)aabb.size(), 0, 0);
}

void Renderer::UpdateLightBuffer(const DirectionalLight& light, const Vect4f& direction, UINT frameIndex, UINT& startLocation)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->GetCbvSrvUavWrapper().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = m_framework->GetCbvSrvUavWrapper().DESCRIPTOR_SIZE();

	LightBuffer::Data lightData;
	lightData.m_ambientColor   = light.m_ambientColor;
	lightData.m_lightColor     = light.m_lightColor;
	lightData.m_lightDirection = direction;

	m_lightBuffer.UpdateBuffer(&lightData, frameIndex);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_lightBuffer.OffsetID() + frameIndex, cbvSrvDescSize);
	m_commandList->SetGraphicsRootDescriptorTable(startLocation, cbvHandle);
	startLocation++;
}
