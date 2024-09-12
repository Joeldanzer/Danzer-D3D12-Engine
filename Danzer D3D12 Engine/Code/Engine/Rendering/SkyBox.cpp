#include "stdafx.h"
#include "SkyBox.h"

#include "PSOHandler.h"
#include "TextureHandler.h"

Skybox::~Skybox(){}
void Skybox::Init(PSOHandler& psoHandler, UINT cubeModel, std::wstring skyBoxTexture, bool spin, std::string skyboxName)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(1, 1, PSOHandler::SAMPLER_WRAP, flags, L"Skybox Root Signature");
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

	m_spin = spin;
	m_skyboxes.emplace(skyboxName, m_textureHandler.CreateTexture(skyBoxTexture, true));
	m_currentSkyBox = m_skyboxes[skyboxName];
	m_cube = cubeModel;
}

void Skybox::Update(const float dt)
{
	if (m_spin) {
		//m_rotation *= Quat4f::CreateFromAxisAngle({ 0.65f, 1.f, 0.2f }, dt * 0.5f);
	}
}

void Skybox::AddSkybox(std::wstring skyboxTexture, std::string skyboxName)
{
	m_skyboxes.emplace(skyboxName, m_textureHandler.CreateTexture(skyboxTexture, true));
	m_textureHandler.LoadAllCreatedTexuresToGPU();
}
