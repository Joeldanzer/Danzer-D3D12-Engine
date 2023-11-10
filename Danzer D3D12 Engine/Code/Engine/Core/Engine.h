#pragma once

class ModelHandler;
class SpriteHandler;
class RenderManager;
class DirectX12Framework;
class SceneManager;
class LevelLoaderCustom;
class TextureHandler;
class CollisionManager;
class Skybox;

class Engine
{
public:
	Engine();
	Engine(unsigned int width, unsigned int height);
	~Engine();

	bool StartEngine(bool editor = true/*Some components perhaps...*/);

	void Update();
	void LateUpdate();

	const float			GetFPS()			  const noexcept;
	const float		    GetDeltaTime()		  const noexcept;
	SceneManager&	    GetSceneManager()	  const noexcept;
	ModelHandler&		GetModelHandler()	  const noexcept;
	SpriteHandler&		GetSpriteHandler()	  const noexcept;
	RenderManager&	    GetRenderManager()	  const noexcept;
	DirectX12Framework& GetFramework()		  const noexcept;
	TextureHandler&		GetTextureHandler()	  const noexcept;
	CollisionManager&	GetCollisionManager() const noexcept;

private:
	friend class Impl;
	class Impl;
	Impl* m_Impl;
};


