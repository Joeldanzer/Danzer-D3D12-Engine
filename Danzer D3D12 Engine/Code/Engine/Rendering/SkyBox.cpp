#include "stdafx.h"
#include "SkyBox.h"

#include "TextureHandler.h"

Skybox::~Skybox()
{
}

void Skybox::Init(UINT cubeModel, std::wstring skyBoxTexture, bool spin, std::string skyboxName)
{
	m_spin = spin;
	m_skyboxes.emplace(skyboxName, m_textureHandler.CreateTexture(skyBoxTexture, true));
	m_textureHandler.LoadAllCreatedTexuresToGPU();
	m_currentSkyBox = m_skyboxes[skyboxName];
	m_cube = cubeModel;
}

void Skybox::Update(const float dt)
{
	if (m_spin) {
		m_rotation *= Quat4f::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, dt * 0.01f);
	}
}

void Skybox::AddSkybox(std::wstring skyboxTexture, std::string skyboxName)
{
	m_skyboxes.emplace(skyboxName, m_textureHandler.CreateTexture(skyboxTexture, true));
	m_textureHandler.LoadAllCreatedTexuresToGPU();
}
