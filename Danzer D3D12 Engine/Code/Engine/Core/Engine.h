#pragma once

class ModelHandler;
class SpriteHandler;
class RenderManager;
class D3D12Framework;
class SceneManager;
class LevelLoaderCustom;
class PhysicsHandler;
class TextureHandler;
class Skybox;
class D3D12Framework;
class ModelEffectHandler;
class LightHandler;
class SoundEngine;
class TextureRenderingHandler;
class BufferHandler;

/*
* Want to turn Engine into a static class so it can be accessed anywhere without needing to send it through
* function calls.
*/
class Engine
{
public:
	Engine() = delete;
	explicit Engine(unsigned int width, unsigned int height);
	~Engine();

	void BeginFrame();
	void UpdateFrame();

	void EndInitFrame();

	const float				 GetFPS()					  const noexcept;
	const float				 GetDeltaTime()				  const noexcept;
							 							  
	SceneManager&			 GetSceneManager()			  const noexcept;
	ModelHandler&			 GetModelHandler()			  const noexcept;
	SpriteHandler&			 GetSpriteHandler()			  const noexcept;
	RenderManager&			 GetRenderManager()			  const noexcept;
	D3D12Framework&			 GetFramework()				  const noexcept;
	TextureHandler&			 GetTextureHandler()	      const noexcept;
	ModelEffectHandler&		 GetModelEffectHandler()	  const noexcept;
	PhysicsHandler&			 GetPhysicsHandler()		  const noexcept;
	SoundEngine&			 GetSoundEngine()		      const noexcept;
	BufferHandler&		     GetBufferHandler()		      const noexcept;
	TextureRenderingHandler& GetTextureRenderingHandler() const noexcept;
	

private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};


