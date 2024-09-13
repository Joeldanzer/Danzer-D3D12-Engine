#include "stdafx.h"
#include "SkyBox.h"

#include "PSOHandler.h"
#include "TextureHandler.h"
#include "Models/ModelHandler.h"

#include "Components/Transform.h"

Skybox::~Skybox(){}

void Skybox::Initialize(PSOHandler& psoHandler, ModelHandler& modelHandler, TextureHandler& textureHandler, std::wstring skyBoxTexture)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	m_rs = psoHandler.CreateRootSignature(1, 1, PSOHandler::SAMPLER_WRAP, flags, L"Skybox Root Signature");
	m_pso = psoHandler.CreatePSO(
		{ L"Shaders/skyboxVS.cso", L"Shaders/skyboxPS.cso" },
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_NONE,
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::IL_INSTANCE_FORWARD,
		L"Skybox PSO"
	);

	m_modelID = modelHandler.CreateCustomModel(ModelData::GetCube()).m_modelID;
	m_textureID = textureHandler.GetTexture(skyBoxTexture);
}

void Skybox::Update(const float dt)
{
	if (m_spin) {
		//m_rotation *= Quat4f::CreateFromAxisAngle({ 0.65f, 1.f, 0.2f }, dt * 0.5f);
	}
}

void Skybox::RenderSkybox(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler, DescriptorHeapWrapper& wrapper, Texture& texture, ModelData& model, const Transform& camTransform, const uint32_t bufferOffset, const uint8_t frameIndex)
{
	cmdList->SetGraphicsRootSignature(psoHandler.GetRootSignature(m_rs));
	cmdList->SetPipelineState(psoHandler.GetPipelineState(m_pso));

	D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle = wrapper.GET_GPU_DESCRIPTOR(bufferOffset + frameIndex);
	cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = wrapper.GET_GPU_DESCRIPTOR(texture.m_offsetID);
	cmdList->SetGraphicsRootDescriptorTable(1, srvHandle);

	if (!model.GetInstanceTransforms().empty())
		 model.ClearInstanceTransform();

	//* Add the rotation from the skybox and always place it a the cameras position
	DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&m_rotation);
	Mat4f transform = DirectX::XMMatrixRotationQuaternion(quatv);
	transform *= DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f);
	transform *= DirectX::XMMatrixTranslation(camTransform.m_position.x, camTransform.m_position.y, camTransform.m_position.z);

	model.AddInstanceTransform(DirectX::XMMatrixTranspose(transform));
	model.UpdateTransformInstanceBuffer(frameIndex);
	D3D12_VERTEX_BUFFER_VIEW bufferView[2] = {
		model.GetMeshes()[0].m_vertexBufferView, model.GetTransformInstanceBuffer().GetBufferView(frameIndex)
	};
	cmdList->IASetVertexBuffers(0, 2, &bufferView[0]);
	cmdList->IASetIndexBuffer(&model.GetMeshes()[0].m_indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawIndexedInstanced(model.GetMeshes()[0].m_numIndices, 1, 0, 0, 0);
}
