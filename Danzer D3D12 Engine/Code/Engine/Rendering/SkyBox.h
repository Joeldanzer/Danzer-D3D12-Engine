#pragma once
#include "Core/D3D12Header.h"
#include "Core/MathDefinitions.h"

#include <array>
#include <string>
#include <unordered_map>

class TextureHandler;

class Skybox
{
public:
	Skybox(TextureHandler& textureHandler) : 
		m_textureHandler(textureHandler),
		m_cube(0),
		m_spin(false),
		m_currentSkyBox(0),
		m_rotation(0.f, 0.f, 0.f, 1.f)
	{}
	~Skybox();

	struct Data {
		UINT m_texture;
		std::string m_skyboxName;
	};

	void Init(UINT cubeModel, std::wstring skyBoxTexture = L"Sprites/defaultRedSkybox.dds", bool spin = false, std::string skyboxName = "defaultSkybox");

	void Update(const float dt);

	void AddSkybox(std::wstring skyboxTexture, std::string skyboxName);
	void SetCurrentSkybox(std::string skyboxName) {
		if (m_skyboxes.find(skyboxName) != m_skyboxes.end())
			m_currentSkyBox = m_skyboxes[skyboxName];
		else
			m_currentSkyBox = m_skyboxes["defaultSkybox"];
		
	}

	UINT GetSkyboxCube() {
		return m_cube;
	}
	UINT GetCurrentSkyboxTexture() {
		return m_currentSkyBox;
	}

	std::array<UINT, 2> GetCurrentActiveSkybox() const { return {m_cube, m_currentSkyBox}; }
	Quat4f& GetRotation() {
		return m_rotation;
	}

private:
	TextureHandler& m_textureHandler;

	std::unordered_map<std::string, UINT> m_skyboxes;
	
	Quat4f m_rotation;
	bool m_spin;

	UINT m_cube;
	UINT m_currentSkyBox;
};

