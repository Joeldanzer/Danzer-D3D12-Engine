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
class DirectX12Framework;

class Transform;

class Renderer
{
public:
	Renderer() : 
		m_descriptorIndex(0),
		m_framework(nullptr),
		m_commandList(nullptr),				 
		m_rootSignature(nullptr), 
	 	m_cameraBuffer(CameraBuffer()),
		m_transformBuffer(TransformBuffer()),
		m_lightBuffer(LightBuffer())
	{}
	~Renderer();

	void Init(DirectX12Framework& framework);

	void UpdateDefaultBuffers(Camera& camera, Transform&, UINT frameIndex);
	void RenderSkybox(Transform& cameraTransform, TextureHandler::Texture& textures, TextureHandler::Texture& textures1, ModelData& model, Skybox& skybox, UINT frameIndex);
	void RenderDirectionalLight(DirectionalLight& light, Vect4f direction, TextureHandler::Texture& skyboxTexture, UINT frameIndex);
	void DefaultRender(std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures);
	//void TransparentRender(Scene* scene, std::vector<Object*>& objects, std::vector<ModelData>& transparentModels, UINT frameIndex, std::vector<TextureHandler::Texture> textures);
	void RenderToGbuffer(std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures);

	void RayRendering(std::vector<RayBuffer::RayInstance >& rays, UINT frameIndex);
	void AABBRendering(std::vector<AABBBuffer::AABBInstance>& aabb, UINT frameIndex);

private:
	void SetDescriptorHeaps(ID3D12DescriptorHeap** descriptorHeaps, UINT count, UINT descriptorIndex = 0);

	UINT m_descriptorIndex;

	DirectX12Framework* m_framework;

	AABBBuffer m_aabbBuffer;
	RayBuffer m_rayBuffer;
	CameraBuffer m_cameraBuffer;
	TransformBuffer m_transformBuffer;
	LightBuffer m_lightBuffer;
	MaterialBuffer  m_materialBuffer;

	ID3D12GraphicsCommandList* m_commandList;
	ID3D12RootSignature*       m_rootSignature;
};

