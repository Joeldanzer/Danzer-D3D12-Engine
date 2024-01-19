#pragma once
#include "Models/ModelData.h"  
#include "Buffers/TransformBuffer.h"
#include "Models/ModelEffectData.h"

#include "Rendering/TextureHandler.h"

#include "Buffers/MaterialBuffer.h"
#include "Buffers/AABBBuffer.h"
#include "Buffers/RayBuffer.h"
#include "Buffers/CameraBuffer.h"
#include "Buffers/LightBuffer.h"
#include "Buffers/EffectShaderBuffer.h"

class Scene;
class Camera; 
class Object;
class Skybox;
class DirectionalLight;
class D3D12Framework;
class DirectionalShadowMapping;
class ModelHandler;

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
	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateShadowMapBuffer(Mat4f& projection, Transform& transform, UINT frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateLightBuffer(Mat4f& projection, Transform& transform, const DirectionalLight& light, const Vect4f& direction, UINT frameIndex);
	
	void RenderToGbuffer(ID3D12GraphicsCommandList* cmdList, std::vector<ModelData>& models, UINT frameIndex, std::vector<TextureHandler::Texture>& textures, bool renderTransparency, UINT startLocation);
	void RenderSkybox(ID3D12GraphicsCommandList* cmdList, Transform& cameraTransform, TextureHandler::Texture& textures, ModelData& model, Skybox& skybox, UINT frameIndex, UINT StartLocation);
	void RenderDirectionalLight(ID3D12GraphicsCommandList* cmdList, TextureHandler::Texture& skyboxTexture, DirectionalShadowMapping& shadowMap, UINT frameIndex, UINT& startLocation);

	void RenderForwardModelEffects(ID3D12GraphicsCommandList* cmdList, const UINT depthOffset, std::vector<ModelEffectData>& modelEffects, ModelHandler& modelHandler, std::vector<TextureHandler::Texture>& textures, const UINT frameIndex, Camera& cam, Transform& camTransform, UINT startLocation);
	//void TransparentRender(Scene* scene, std::vector<ModelData>& transparentModels, UINT frameIndex, std::vector<TextureHandler::Texture> textures);
	//void RayRendering(std::vector<RayBuffer::RayInstance>& rays, UINT frameIndex);
	//void AABBRendering(std::vector<AABBBuffer::AABBInstance>& aabb, UINT frameIndex);

private:
	UINT m_descriptorIndex;

	D3D12Framework* m_framework;

	AABBBuffer m_aabbBuffer;
	RayBuffer m_rayBuffer;
	CameraBuffer m_cameraBuffer;
	CameraBuffer m_shadowBuffer;
	TransformBuffer m_transformBuffer;
	LightBuffer m_lightBuffer;
	MaterialBuffer  m_materialBuffer;
	EffectShaderBuffer m_effectBuffer;

	//ID3D12GraphicsCommandList* m_commandList;
	ID3D12RootSignature*       m_rootSignature;
};

