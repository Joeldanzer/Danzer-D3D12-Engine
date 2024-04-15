#pragma once
#include "../Core/D3D12Header.h"

#include "Renderer.h"
#include "2D/2DRenderer.h"

#include <map>

class Scene;
class Skybox;
class ModelHandler;
class SpriteHandler;
class TextureHandler;
class D3D12Framework;
class ModelEffectHandler;
class LightHandler;
class PSOHandler;

struct Object;
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
		SpriteHandler& SpriteHandler, Skybox& skybox, Scene& scene/*Camera, Ligthing, GameObjects, etc...*/);

	PSOHandler& GetPSOHandler() const noexcept;

private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};

