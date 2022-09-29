#include "stdafx.h"
#include "Renderer.h"

#include "Scene.h"
#include "Components/Transform.h";
#include "Components/Object.h"
#include "Components/DirectionalLight.h"
#include "SkyBox.h"

#include "Camera.h"

#include "RenderUtility.h"


#include "Core/DirectX12Framework.h"

#include <string>

Renderer::~Renderer()
{
	m_cameraBuffer.~CameraBuffer();
	m_transformBuffer.~TransformBuffer();

	m_commandList = nullptr;
	m_rootSignature = nullptr;
}

void Renderer::Init(DirectX12Framework& framework)
{
	m_commandList = framework.GetCommandList();
	m_cameraBuffer.Init(framework.GetDevice());
	m_aabbBuffer.Init(framework.GetDevice());
	m_rayBuffer.Init(framework.GetDevice());
	m_lightBuffer.Init(framework.GetDevice());
	m_materialBuffer.Init(framework.GetDevice());

	m_framework = &framework;
}

//* Default Buffers for all existing 3D models. Should only be updated once per frame!
void Renderer::UpdateDefaultBuffers(Camera& camera, Transform& transform, UINT frameIndex)
{
	// Reset descriptor index! 
	m_descriptorIndex = 0;
	
	CameraBuffer::Data bufferData;
	bufferData.m_transform  = transform.GetWorld().Invert();
	bufferData.m_projection = camera.GetProjection();
	bufferData.m_position = { transform.m_position.x, transform.m_position.y, transform.m_position.z, float(camera.m_renderTarget)};
	Vect4f eye = { bufferData.m_transform.Forward() };
	eye.w = 1.f;
	bufferData.m_direction = eye;

	m_cameraBuffer.UpdateBuffer(frameIndex, &bufferData);

	ID3D12DescriptorHeap* descHeaps[] = { m_cameraBuffer.GetDescriptorHeap(frameIndex) };
	m_commandList->SetDescriptorHeaps(1, &descHeaps[0]);
	m_commandList->SetGraphicsRootDescriptorTable(0, descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
}

void Renderer::RenderSkybox(Transform& cameraTransform, TextureHandler::Texture& textures, ModelData& model, Skybox& skybox, UINT frameIndex)
{
	UINT oldDescripterIndex = m_descriptorIndex;
	
	//ID3D12DescriptorHeap* descHeaps1[1] = { textures1.m_textureDescriptorHeap.Get() };
	ID3D12DescriptorHeap* descHeaps[1] = {textures.m_textureDescriptorHeap.Get()};
	
	//m_commandList->SetDescriptorHeaps(_countof(descHeaps), &descHeaps[0]);
	//m_commandList->SetGraphicsRootDescriptorTable(0, textures1.m_textureDescriptorHeap->GetGPUDescriptorHandleForHeapStart());	
	
	m_commandList->SetDescriptorHeaps(_countof(descHeaps), &descHeaps[0]);
	CD3DX12_GPU_DESCRIPTOR_HANDLE descHandle ;
	m_commandList->SetGraphicsRootDescriptorTable(1, textures.m_textureDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	ModelData::Mesh& mesh = model.GetMeshes()[0];

	if (!model.GetInstanceTransforms().empty())
		model.ClearInstanceTransform();

	Mat4f transform = Mat4f::CreateScale({ 5.f, 5.f, 5.f });
	transform *= Mat4f::CreateFromQuaternion(skybox.GetRotation());
	transform.Translation(cameraTransform.m_position);

	model.AddInstanceTransform(transform);
	model.UpdateTransformInstanceBuffer(frameIndex);
	D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
		mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
	};
	m_commandList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
	m_commandList->IASetIndexBuffer(&mesh.m_indexBufferView);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_commandList->DrawIndexedInstanced(mesh.m_numIndices, 1, 0, 0, 0);

	model.ClearInstanceTransform();
}

void Renderer::RenderDirectionalLight(DirectionalLight& light, Vect4f direction, TextureHandler::Texture& skyboxTexture, UINT frameIndex)
{
	LightBuffer::Data lightData;
	lightData.m_ambientColor   = light.m_ambientColor;
	lightData.m_lightColor	   = light.m_lightColor;
	lightData.m_lightDirection = direction;

	m_lightBuffer.UpdateBuffer(frameIndex, &lightData);

	ID3D12DescriptorHeap* descHeap = m_lightBuffer.GetDescriptorHeap(frameIndex);
	m_commandList->SetDescriptorHeaps(1, &descHeap);
	m_commandList->SetGraphicsRootDescriptorTable(1, descHeap->GetGPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* skyboxTextureHeap = skyboxTexture.m_textureDescriptorHeap.Get();
	m_commandList->SetDescriptorHeaps(1, &skyboxTextureHeap);
	m_commandList->SetGraphicsRootDescriptorTable(3, skyboxTextureHeap->GetGPUDescriptorHandleForHeapStart());

	m_commandList->IASetVertexBuffers(0, 0, nullptr);
	m_commandList->IASetIndexBuffer(nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	m_commandList->DrawInstanced(3, 1, 0, 0);
}
//*Simple Renderer that doesnt require any lighting or anything else
void Renderer::DefaultRender(std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures)
{
	//for (UINT i = 0; i < models.size(); i++)
	//{
	//	if (!models[i].IsTransparent()) {
	//
	//		ModelData& model = models[i];
	//		
	//		UINT numberOfTransforms = ((UINT)model.GetInstanceTransforms().size() < MAX_INSTANCES_PER_MODEL) ? (UINT)model.GetInstanceTransforms().size() : MAX_INSTANCES_PER_MODEL;
	//
	//		if (numberOfTransforms > 0) {
	//			model.UpdateTransformInstanceBuffer(frameIndex);
	//			
	//			for (ModelData::Mesh& mesh : model.GetMeshes())
	//			{
	//				if (mesh.m_renderMesh) {
	//					//Before rendering save current Descriptor index
	//					UINT oldDescripterIndex = m_descriptorIndex;
	//
	//					//std::vector<ID3D12DescriptorHeap*> descHeaps;
	//					//for (UINT heaps = 0; heaps < mesh.m_textures.size(); heaps++)
	//					//	descHeaps.emplace_back(mesh.m_textures[heaps].m_textureDescriptorHeap.Get());
	//					
	//					ID3D12DescriptorHeap* descHeaps[] = { textures[mesh.m_texture - 1].m_textureDescriptorHeap.Get()};
	//					m_commandList->SetDescriptorHeaps(1, &descHeaps[0]);
	//					m_commandList->SetGraphicsRootDescriptorTable(2, descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
	//					//SetDescriptorHeaps(&descHeaps[0], 1, oldDescripterIndex);
	//
	//					D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
	//						mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
	//					};
	//					m_commandList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
	//					m_commandList->IASetIndexBuffer(&mesh.m_indexBufferView);
	//					m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//					m_commandList->DrawIndexedInstanced(mesh.m_numIndices, numberOfTransforms, 0, 0, 0);
	//				}			
	//			}			
	//		}
	//	}
	//}
}

void Renderer::RenderToGbuffer(std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures)
{
	for (UINT i = 0; i < models.size(); i++)
	{
		if (!models[i].IsTransparent()) {

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
						materialData.m_hasMaterialTexture = mesh.m_material.m_metallic > 0 ? 1 : 0;
					

						mesh.m_materialBuffer.UpdateBuffer(frameIndex, &materialData);
						ID3D12DescriptorHeap* cbvDescHeap = mesh.m_materialBuffer.GetDescriptorHeap(frameIndex);
						m_commandList->SetDescriptorHeaps(1, &cbvDescHeap);
						m_commandList->SetGraphicsRootDescriptorTable(1, cbvDescHeap->GetGPUDescriptorHandleForHeapStart());
						
						ID3D12DescriptorHeap* albedoHeap;
						ID3D12DescriptorHeap* normalHeap;
						ID3D12DescriptorHeap* materialHeap;

						UINT albedo   = mesh.m_material.m_albedo;
						UINT normal   = mesh.m_material.m_normal;
						UINT metallic = mesh.m_material.m_metallic;

						if (albedo == 0)
							albedoHeap = textures[0].m_textureDescriptorHeap.Get();
						else
							albedoHeap = textures[albedo - 1].m_textureDescriptorHeap.Get();

						if (normal == 0)
							normalHeap = textures[0].m_textureDescriptorHeap.Get();
						else
							normalHeap = textures[normal - 1].m_textureDescriptorHeap.Get();

						if (metallic == 0)
							materialHeap = textures[0].m_textureDescriptorHeap.Get();
						else
							materialHeap = textures[metallic - 1].m_textureDescriptorHeap.Get();

						m_commandList->SetDescriptorHeaps(1, &albedoHeap);
						m_commandList->SetGraphicsRootDescriptorTable(2, albedoHeap->GetGPUDescriptorHandleForHeapStart());
						m_commandList->SetDescriptorHeaps(1, &normalHeap);
						m_commandList->SetGraphicsRootDescriptorTable(3, normalHeap->GetGPUDescriptorHandleForHeapStart());
						m_commandList->SetDescriptorHeaps(1, &materialHeap);
						m_commandList->SetGraphicsRootDescriptorTable(4, materialHeap->GetGPUDescriptorHandleForHeapStart());
						
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

//*Transparent 3D objects needs to be rendered individually in distance order,
//*like what a depth buffer does but manually.
//void Renderer::TransparentRender(Scene* scene, std::vector<Object*>& objects, std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture> textures)
//{
//	for (UINT i = 0; i < objects.size(); i++)
//	{
//		if (models[objects[i]->GetModel() - 1].IsTransparent()) {
//			ModelData& model = models[objects[i]->GetModel() - 1];
//
//			model.AddInstanceTransform(objects[i]->GetTransform() * scene->GetLayer(objects[i]->GetLayer()).m_layerTransform);
//			model.UpdateTransformInstanceBuffer(frameIndex);
//			
//			for (ModelData::Mesh& mesh : model.GetMeshes())
//			{
//				if (mesh.m_renderMesh) {
//					//Before rendering save current Descriptor index
//					UINT oldDescripterIndex = m_descriptorIndex;
//			
//					ID3D12DescriptorHeap* descHeaps[] = { textures[mesh.m_texture - 1].m_textureDescriptorHeap.Get() };
//					m_commandList->SetDescriptorHeaps(1, &descHeaps[0]);
//					m_commandList->SetGraphicsRootDescriptorTable(2, descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
//
//					D3D12_VERTEX_BUFFER_VIEW vBufferViews[2] = {
//						mesh.m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
//					};
//
//					m_commandList->IASetVertexBuffers(0, 2, &vBufferViews[0]);
//					m_commandList->IASetIndexBuffer(&mesh.m_indexBufferView);
//					m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//			
//					m_commandList->DrawIndexedInstanced(mesh.m_numIndices, 1, 0, 0, model.GetInstanceTransforms().size() - 1);
//				}
//			}
//		}
//	}
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

//* Use to make it easier to keep track on setting DescriptorHeaps as it can get a bit confusing when CBV/SRV/UAV
//* All share the same SetGraphicsRootDescriptorTable, do remember that PipelineStateHandler is where the resource slots
//* of the specific resources that is used are located. 
void Renderer::SetDescriptorHeaps(ID3D12DescriptorHeap** descriptorHeaps, UINT count, UINT descriptorIndex)
{
	m_commandList->SetDescriptorHeaps(count, descriptorHeaps);
	for (UINT i = 0; i < count; i++)
	{
		if (descriptorHeaps[i]) {
			m_commandList->SetGraphicsRootDescriptorTable(m_descriptorIndex, descriptorHeaps[i]->GetGPUDescriptorHandleForHeapStart());
		}
		m_descriptorIndex++;
	}

	if(descriptorIndex != 0)
		m_descriptorIndex = descriptorIndex;
}
