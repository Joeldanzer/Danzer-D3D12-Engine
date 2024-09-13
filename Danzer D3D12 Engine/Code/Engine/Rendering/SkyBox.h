#pragma once

class ModelHandler;
class TextureHandler;
class PSOHandler;
class DescriptorHeapWrapper;

class  Transform;
class  ModelData;
struct Texture;
struct ID3D12GraphicsCommandList;

class Skybox
{
public:
	Skybox() : 
		m_spin(false),
		m_rotation(0.f, 0.f, 0.f, 1.f),
		m_pso(0),
		m_rs(0),
		m_textureID(0),
		m_modelID(0)
	{}
	~Skybox();

	void Initialize(PSOHandler& psoHandler, ModelHandler& modelHandler, TextureHandler& textureHandler, std::wstring skyBoxTexture = L"Sprites/defaultRedSkybox.dds");
	void Update(const float dt);
	void RenderSkybox(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler, DescriptorHeapWrapper& wrapper, Texture& texture, ModelData& model, const Transform& camTransform, const uint32_t bufferOffset, const uint8_t frameIndex);

	const uint32_t TextureID() { return m_textureID; }
	const uint32_t ModelID()   { return m_modelID; }

	Quat4f& GetRotation() {
		return m_rotation;
	}

private:
	Quat4f m_rotation;
	bool m_spin;

	uint32_t m_pso, m_rs, m_textureID, m_modelID;
};

