#pragma once
#include "entt/entt.hpp"
#include "Models/ModelData.h"  
#include "Buffers/VertexBuffer.h"
#include "Models/ModelEffectData.h"

#include "Rendering/TextureHandler.h"

#include "Buffers/MaterialBuffer.h"
#include "Buffers/CameraBuffer.h"
#include "Buffers/LightBuffer.h"
#include "Buffers/EffectShaderBuffer.h"
#include "Buffers/PointLightBuffer.h"

class Scene;
class Camera; 
class Skybox;
class D3D12Framework;
class DirectionalShadowMapping;
class SSAOTexture;
class ModelHandler;
class LightHandler;
class PSOHandler;
class FullscreenTexture;
class Transform;

struct DirectionalLight;
struct GameEntity;

// This class is very outdated and I am unsure if i will be using it now that TextureRenderingHandler Exists....
class Renderer
{
public:
	Renderer() : 
		m_descriptorIndex(0),
		m_framework(nullptr),	 
		m_rootSignature(nullptr), 
	 	m_cameraBuffer(CameraBuffer()),
		m_transformBuffer(VertexBuffer()),
		m_lightBuffer(LightBuffer()),
		m_pointLightBuffer(PointLightBuffer())
	{}
	~Renderer();

	void Init(D3D12Framework& framework);

	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateDefaultBuffers(Camera& camera, Transform&, UINT frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateShadowMapBuffer(const Mat4f& projection, const Mat4f& transform, const Vect4f& position, UINT frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE UpdateLightBuffer(Mat4f& projection, Transform& transform, const DirectionalLight& light, const Vect4f& direction, UINT frameIndex);
	
	void RenderToGbuffer(ID3D12GraphicsCommandList* cmdList, std::vector<ModelData>& models, UINT frameIndex, std::vector<Texture>& textures, bool renderTransparency, UINT startLocation);
	void RenderForwardModelEffects(ID3D12GraphicsCommandList* cmdList, PSOHandler& psoHandler, const UINT depthOffset, std::vector<ModelEffectData>& modelEffects, ModelHandler& modelHandler, std::vector<Texture>& textures, const UINT frameIndex, Camera& cam, Transform& camTransform, UINT startLocation);
private:
	UINT m_descriptorIndex;

	D3D12Framework* m_framework;

	CameraBuffer	   m_cameraBuffer;
	CameraBuffer	   m_shadowBuffer;
	VertexBuffer       m_transformBuffer;
	LightBuffer		   m_lightBuffer;
	MaterialBuffer     m_materialBuffer;
	EffectShaderBuffer m_effectBuffer;
	PointLightBuffer   m_pointLightBuffer;

	ID3D12RootSignature*       m_rootSignature;
};

