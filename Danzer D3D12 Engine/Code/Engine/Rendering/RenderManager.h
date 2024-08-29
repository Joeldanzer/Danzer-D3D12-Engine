#pragma once
#include "../Core/D3D12Header.h"

#include "Renderer.h"
#include "2D/2DRenderer.h"

#include <map>

class Skybox;
class SceneManager;
class ModelHandler;
class SpriteHandler;
class TextureHandler;
class D3D12Framework;
class ModelEffectHandler;
class LightHandler;
class PSOHandler;
class VolumetricLight;

struct GameEntity;
struct Sprite;
struct Text;
struct Transform2D;

class RenderManager
{
public:
	RenderManager() = delete;
	RenderManager(D3D12Framework& framework, TextureHandler& textureHandler);
	~RenderManager();

	void BeginFrame();
	void RenderFrame(LightHandler& lightHandler, TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler,
		SpriteHandler& SpriteHandler, Skybox& skybox, SceneManager& scene/*Camera, Ligthing, GameObjects, etc...*/);

	PSOHandler& GetPSOHandler() const noexcept;
	VolumetricLight& GetVolumetricLight() const noexcept;
	void SetKuwaharaRadius(UINT radius, UINT scale, Vect3f offset);

private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};

