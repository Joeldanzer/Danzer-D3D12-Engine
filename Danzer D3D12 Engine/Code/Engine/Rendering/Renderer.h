#pragma once
#include "Models/ModelData.h"
#include "Models/TransformBuffer.h"

#include "Rendering/TextureHandler.h"

#include "Buffers/MaterialBuffer.h"
#include "Buffers/AABBBuffer.h"
#include "Buffers/RayBuffer.h"
#include "Buffers/CameraBuffer.h"
#include "Buffers/LightBuffer.h"

class Scene;
class Camera; 
class Object;
class Skybox;
class DirectionalLight;
class D3D12Framework;

class Transform;

class Renderer
{
public:
	Renderer() : 
		m_descriptorIndex(0),
		m_framework(nullptr),	 
		m_rootSignature(nullptr), 
	 	m_cameraBuffer(CameraBuffer()),
		m_transformBuffer(TransformBuffer()),
		m_lightBuffer(LightBuffer())
	{}
	~Renderer();

	void Init(D3D12Framework& framework);

	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateDefaultBuffers(Camera& camera, Transform&, UINT frameIndex);
	void RenderSkybox(ID3D12GraphicsCommandList* cmdList, Transform& cameraTransform, TextureHandler::Texture& textures, ModelData& model, Skybox& skybox, UINT frameIndex, UINT StartLocation);
	void RenderDirectionalLight(ID3D12GraphicsCommandList* cmdList, TextureHandler::Texture& skyboxTexture, UINT frameIndex, UINT& startLocation);
	//void TransparentRender(Scene* scene, std::vector<ModelData>& transparentModels, UINT frameIndex, std::vector<TextureHandler::Texture> textures);
	void RenderToGbuffer(ID3D12GraphicsCommandList* cmdList, std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures, bool renderTransparency, UINT startLocation);

	//void RayRendering(std::vector<RayBuffer::RayInstance>& rays, UINT frameIndex);
	//void AABBRendering(std::vector<AABBBuffer::AABBInstance>& aabb, UINT frameIndex);

	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateLightBuffer(const DirectionalLight& light, const Vect4f& direction, UINT frameIndex);

private:
	UINT m_descriptorIndex;

	D3D12Framework* m_framework;

	AABBBuffer m_aabbBuffer;
	RayBuffer m_rayBuffer;
	CameraBuffer m_cameraBuffer;
	TransformBuffer m_transformBuffer;
	LightBuffer m_lightBuffer;
	MaterialBuffer  m_materialBuffer;

	//ID3D12GraphicsCommandList* m_commandList;
	ID3D12RootSignature*       m_rootSignature;
};

