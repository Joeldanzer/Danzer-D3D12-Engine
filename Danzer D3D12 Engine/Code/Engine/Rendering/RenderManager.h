#pragma once

class Skybox;
class SceneManager;
class ModelHandler;
class SpriteHandler;
class TextureHandler;
class D3D12Framework;
class ModelEffectHandler;
class PSOHandler;
class TextureRenderingHandler;
class BufferHandler;
class Skybox;

struct GameEntity;
struct Sprite;
struct TextOLD;
struct Transform2D;

class RenderManager
{
public:
	RenderManager() = delete;
	RenderManager(D3D12Framework& framework, TextureHandler& textureHandler);
	~RenderManager();

	void InitializeRenderTextures(TextureHandler& textureHandler, ModelHandler& modelHandler);

	void BeginFrame();
	void RenderFrame(TextureHandler& textureHandler, ModelHandler& modelHandler, ModelEffectHandler& effectHandler,
		SpriteHandler& SpriteHandler, SceneManager& scene/*Camera, Ligthing, GameObjects, etc...*/);

	PSOHandler&				 GetPSOHandler()	   const noexcept;
	BufferHandler&		     GetConstantHandler()  const noexcept;
	TextureRenderingHandler& GetTextureRendering() const noexcept;

	void SetKuwaharaRadius(UINT radius, UINT scale, Vect3f offset);
	
private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};

