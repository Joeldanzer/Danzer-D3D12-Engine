#pragma once
#include "WindowHandler.h"

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

class Engine
{
public:
	Engine() = delete;
	explicit Engine(unsigned int width, unsigned int height);
	~Engine();

	static Engine& GetInstance() {
		if (s_engineSingleton == nullptr)
			s_engineSingleton = new Engine(WindowHandler::WindowData().m_w, WindowHandler::WindowData().m_h);
		
		return *s_engineSingleton;
	}

	// These should not be accesible

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
	static Engine* s_engineSingleton;
	
	void EndInitFrame();
	void BeginFrame();
	void UpdateFrame();

	friend class Launcher;
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};


