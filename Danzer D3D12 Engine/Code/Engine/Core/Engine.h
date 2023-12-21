#pragma once

class ModelHandler;
class SpriteHandler;
class RenderManager;
class D3D12Framework;
class SceneManager;
class LevelLoaderCustom;
class TextureHandler;
class CollisionManager;
class Skybox;
class D3D12Framework;
class ModelEffectHandler;

class Engine
{
public:
	Engine();
	Engine(unsigned int width, unsigned int height);
	~Engine();

	bool StartEngine(bool editor = true/*Some components perhaps...*/);

	void BeginUpdate();
	void MidUpdate();
	void LateUpdate();

	void EndInitFrame();

	const float			GetFPS()			    const noexcept;
	const float		    GetDeltaTime()		    const noexcept;
	SceneManager&	    GetSceneManager()	    const noexcept;
	ModelHandler&		GetModelHandler()	    const noexcept;
	SpriteHandler&		GetSpriteHandler()	    const noexcept;
	RenderManager&	    GetRenderManager()	    const noexcept;
	D3D12Framework&		GetFramework()		    const noexcept;
	TextureHandler&		GetTextureHandler()	    const noexcept;
	CollisionManager&	GetCollisionManager()   const noexcept;
	ModelEffectHandler& GetModelEffectHandler() const noexcept;
	

private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};


