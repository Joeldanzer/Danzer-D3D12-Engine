#pragma once
#include "../Core/D3D12Header.h"

#include "Renderer.h"
#include "2D/2DRenderer.h"
#include "PipelineStateHandler.h"

#include <map>

class Scene;
class Skybox;
class ModelHandler;
class SpriteHandler;
class TextureHandler;
class D3D12Framework;
class ModelEffectHandler;

struct Object;
struct Sprite;
struct Text;
struct Transform2D;

class RenderManager
{
public:
	RenderManager() = delete;
	RenderManager(D3D12Framework& framework);
	~RenderManager();

	void BeginFrame();
	void RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler,
		SpriteHandler& SpriteHandler, Skybox& skybox, Scene& scene/*Camera, Ligthing, GameObjects, etc...*/);


private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};

