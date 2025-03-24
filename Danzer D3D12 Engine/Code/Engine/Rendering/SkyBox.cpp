#include "stdafx.h"
#include "SkyBox.h"

#include "PSOHandler.h"
#include "Core/Engine.h"
#include "TextureHandler.h"
#include "Models/ModelHandler.h"

#include "Components/Transform.h"

Skybox::Skybox(ModelHandler& modelHandler) : 
	m_spin(false),
	m_rotation(0.f, 0.f, 0.f, 1.f),
	m_pso(0),
	m_rs(0),
	m_textureID(0),
	m_modelID(modelHandler.CreateCustomModel(ModelData::GetCube()).m_modelID)
{
}

Skybox::~Skybox(){}

void Skybox::Initialize(PSOHandler& psoHandler, TextureHandler& textureHandler, std::wstring skyBoxTexture)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	InitializeRenderer(
		L"Skybox",
		L"Shaders/skyboxVS.cso",
		L"Shaders/skyboxPS.cso",
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		false,
		depth,
		DXGI_FORMAT_UNKNOWN,
		{ DXGI_FORMAT_R8G8B8A8_UNORM },
		PSOHandler::BLEND_DEFAULT,
		PSOHandler::RASTERIZER_DEFAULT,
		PSOHandler::SAMPLER_WRAP,
		flags,
		1,
		1,
		PSOHandler::IL_INSTANCE_FORWARD,
		psoHandler
	);
	m_textureID = textureHandler.CreateTexture(skyBoxTexture, true);
}

void Skybox::Update(const float dt)
{
	if (m_spin) {
		//m_rotation *= Quat4f::CreateFromAxisAngle({ 0.65f, 1.f, 0.2f }, dt * 0.5f);
	}
}

bool Skybox::RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& heap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex)
{
	if (!m_modelData->GetSingleMesh(0).m_instanceTransforms.empty())
		m_modelData->GetSingleMesh(0).m_instanceTransforms.clear();
	
	Texture& texture = Engine::GetInstance().GetTextureHandler().GetTextureData(m_textureID);
	SetTextureAtSlot(texture.m_offsetID, 0, false);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = heap.GET_CPU_DESCRIPTOR(m_rtvSlots[0].first + frameIndex);

	cmdList->RSSetViewports(1, &m_viewPort);
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	//* Add the rotation from the skybox and always place it a the cameras position
	DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&m_rotation);
	Mat4f transform = DirectX::XMMatrixRotationQuaternion(quatv);
	transform *= DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f);
	transform *= DirectX::XMMatrixTranslation(m_camPosition.x, m_camPosition.y, m_camPosition.z);

	ModelData::Mesh& mesh = m_modelData->GetSingleMesh(0);

	m_modelData->AddMeshToRender(0, DirectX::XMMatrixTranspose(transform));
	m_modelData->UpdateAllMeshInstanceBuffer(frameIndex);
	D3D12_VERTEX_BUFFER_VIEW bufferView[2] = {
		mesh.m_vertexBufferView, mesh.m_meshBuffer.GetBufferView(frameIndex)
	};
	cmdList->IASetVertexBuffers(0, 2, &bufferView[0]);
	cmdList->IASetIndexBuffer(&mesh.m_indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawIndexedInstanced(mesh.m_numIndices, 1, 0, 0, 0);

	m_modelData->GetSingleMesh(0).m_instanceTransforms.clear();

	return true;
}

void Skybox::FetchCameraPositionAndSkyboxModel(const Vect3f position, ModelHandler& modelHandler)
{
	m_modelData   = &modelHandler.GetLoadedModelInformation(m_modelID);
	m_camPosition = position;
}
