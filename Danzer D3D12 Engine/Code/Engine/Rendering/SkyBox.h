#pragma once

#include "Screen Rendering/TextureRenderer.h"

class ModelHandler;
class TextureHandler;
class PSOHandler;
class DescriptorHeapWrapper;

class  Transform;
class  ModelData;
struct Texture;
struct ID3D12GraphicsCommandList;

class Skybox : public TextureRenderer
{
public:
	Skybox(ModelHandler& modelHandler);
	~Skybox();

	void Initialize(PSOHandler& psoHandler, TextureHandler& textureHandler, std::wstring skyBoxTexture = L"Sprites/defaultRedSkybox.dds");
	void Update(const float dt);

	void RenderToTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& heap, DescriptorHeapWrapper& cbvSrvHeap, const uint8_t frameIndex);
	void FetchCameraPositionAndSkyboxModel(const Vect3f position, ModelHandler& modelHandler);

	const uint32_t TextureID() { return m_textureID; }
	const uint32_t ModelID()   { return m_modelID; }

	Quat4f& GetRotation() {
		return m_rotation;
	}

private:
	Quat4f m_rotation;
	bool m_spin;

	Vect3f     m_camPosition;
	ModelData* m_modelData = nullptr;
	uint32_t   m_pso, m_rs, m_textureID, m_modelID;
};

