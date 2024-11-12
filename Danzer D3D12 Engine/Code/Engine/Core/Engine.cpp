#include "stdafx.h"
#include "Engine.h"

#include "Level Loader/LevelLoaderCustom.h"
#include "Components/Transform.h"
#include "Core/input.hpp"
#include "Rendering/TextureHandler.h"
#include "WindowHandler.h"
#include "SceneManager.h"
#include "Rendering/RenderManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/Models/ModelEffectHandler.h"
#include "Rendering/Screen Rendering/LightHandler.h"
#include "Rendering/Buffers/BufferHandler.h"
#include "Rendering/Screen Rendering/Textures/TextureRenderingHandler.h"
#include "Rendering/2D/SpriteHandler.h"
#include "Scene.h"
#include "FrameTimer.h"
#include "Rendering/Camera.h"
#include "D3D12Framework.h"
#include "Physics/PhysicsEngine.h"
#include "Sound/SoundEngine.h"

// ImGui
#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"


class Engine::Impl {
public:
	Impl(unsigned int width, unsigned int height);
	~Impl();

	void BeginUpdate();
	void MidUpdate();
	void LateUpdate();

	void EndInitFrame();

	const float				 GetFPS()					  noexcept;
	const float				 GetDeltaTime()				  noexcept;
	SceneManager&			 GetSceneManager()			  noexcept;
	ModelHandler&			 GetModelFactory()			  noexcept;
	SpriteHandler&			 GetSpriteFactory()			  noexcept;
	RenderManager&			 GetRenderManager()			  noexcept;
	D3D12Framework&			 GetFramework()				  noexcept;
	TextureHandler&			 GetTextureHandler()		  noexcept;
	ModelEffectHandler&		 GetModelEffectHandler()	  noexcept;
	LightHandler&			 GetLightHandler()			  noexcept;
	PhysicsHandler&			 GetPhysicsHandler()		  noexcept;
	SoundEngine&			 GetSoundEngine()			  noexcept;
	BufferHandler&			 GetBufferHandler()			  noexcept;
	TextureRenderingHandler& GetTextureRenderingHandler() noexcept;


private:
	SoundEngine				m_soundEngine;
	WindowHandler			m_windowHandler;
	D3D12Framework			m_framework;
	TextureHandler			m_textureHandler;
	RenderManager			m_renderManager;
	BufferHandler			m_bufferHandler;
	ModelHandler			m_modelHandler;
	SpriteHandler			m_spriteHandler;
	SceneManager			m_sceneManager;
	PhysicsEngine			m_physicsEngine;
	PhysicsHandler			m_physicsHandler;
	ModelEffectHandler		m_modelEffectHandler;
	LightHandler			m_lightHandler;
	FrameTimer				m_frameTimer;
	Camera					m_camera;

	float m_deltaTime;
};

Engine::Impl::Impl(unsigned int width, unsigned int height) :
	m_windowHandler({ 0, 0, width, height }), 
	m_framework(),
	m_textureHandler(m_framework),
	m_renderManager(m_framework, m_textureHandler),
	m_modelHandler(m_framework, m_textureHandler),
	m_modelEffectHandler(m_framework, m_renderManager.GetPSOHandler()),
	m_spriteHandler(m_framework, m_textureHandler),
	m_bufferHandler(m_framework),
	m_lightHandler(m_framework),
	m_sceneManager(m_camera),
	m_physicsEngine(
		10240, // Max number of bodies
		0,     // Max body mutexes
		65536, // Max body pairs
		20480, // Max Contact Constraints
		3      // Max Number of jobs(AKA threads)
	),
	m_physicsHandler(m_physicsEngine),
	m_soundEngine(),
	m_deltaTime(0.f)
{
	m_renderManager.InitializeRenderTextures(m_textureHandler, m_modelHandler);

	ImGuiIO* io = &ImGui::GetIO();
	ImVec2 vec;
	vec.x = (float)width;
	vec.y = (float)height;

	io->DisplaySize = vec;
	io->Fonts->Build();

	m_spriteHandler.CreateSpriteSheet(L"Sprites/testSpriteSheet.dds", 4, 4);

	m_physicsEngine.SetRegistry(m_sceneManager.Registry());
	m_soundEngine.SetRegistry(m_sceneManager.Registry());
}

Engine::Impl::~Impl()
{
	m_sceneManager.~SceneManager();
	m_renderManager.~RenderManager();
	m_windowHandler.~WindowHandler();
	m_modelHandler.~ModelHandler();
	m_spriteHandler.~SpriteHandler();
	m_physicsEngine.~PhysicsEngine();
	m_physicsHandler.~PhysicsHandler();
	m_framework.~D3D12Framework();
}

Engine::Engine(unsigned int width, unsigned int height)
{
	m_Impl = new Engine::Impl(width, height);
}
Engine::~Engine(){
	//delete m_Impl;
}

void Engine::Impl::BeginUpdate()
{
	m_frameTimer.Update();
	const float deltaTime = m_frameTimer.GetRealDeltaTime();

	m_renderManager.BeginFrame();
}

void Engine::Impl::MidUpdate()
{
	const float deltaTime = m_frameTimer.GetRealDeltaTime();

	m_sceneManager.UpdateTransformsForRendering();

	//m_physicsHandler.SetPhysicsPositionAndRotation(m_sceneManager.Registry());
	//m_physicsEngine.Update(1.0f / 60.0f, 0);
	//m_physicsHandler.UpdatePhysicsEntities(m_sceneManager.Registry());

	m_soundEngine.UpdateSound(deltaTime);

	m_renderManager.RenderFrame(m_lightHandler, m_textureHandler, m_modelHandler, m_modelEffectHandler, m_spriteHandler, m_sceneManager);
}

void Engine::Impl::LateUpdate()
{
	m_sceneManager.UpdateLastPositions();

	m_framework.ExecuteCommandList();
	m_framework.GetSwapChain()->Present(1, 0);
	m_framework.WaitForGPU();
}

void Engine::Impl::EndInitFrame()
{
	m_sceneManager.UpdateTransformsForRendering(true);

	m_physicsHandler.UpdateStaticColliders(m_sceneManager.Registry());
	m_physicsEngine.OptimizeBroadPhase();
	m_framework.EndInitFrame();
}

//bool Engine::StartEngine(bool editor)
//{
//	if (editor)
//		s_engineState = EngineState::ENGINE_STATE_EDITOR;
//	else
//		s_engineState = EngineState::ENGINE_STATE_GAME;
//	
//	return true;
//}

void Engine::BeginUpdate()
{
	m_Impl->BeginUpdate();
}

void Engine::MidUpdate()
{
	m_Impl->MidUpdate();
}
void Engine::LateUpdate()
{
	m_Impl->LateUpdate();
}
void Engine::EndInitFrame()
{
	m_Impl->EndInitFrame();
}
const float Engine::GetFPS() const noexcept
{
	return m_Impl->GetFPS();
}
const float Engine::GetDeltaTime() const noexcept
{
	return m_Impl->GetDeltaTime();
}
SceneManager& Engine::GetSceneManager() const noexcept
{
	return m_Impl->GetSceneManager();
}
ModelHandler& Engine::GetModelHandler() const noexcept
{
	return m_Impl->GetModelFactory();
}
SpriteHandler& Engine::GetSpriteHandler() const noexcept
{
	return m_Impl->GetSpriteFactory();
}
RenderManager& Engine::GetRenderManager() const noexcept
{
	return m_Impl->GetRenderManager();
}
D3D12Framework& Engine::GetFramework() const noexcept
{
	return m_Impl->GetFramework();
}

TextureHandler& Engine::GetTextureHandler() const noexcept
{
	return m_Impl->GetTextureHandler();
}

ModelEffectHandler& Engine::GetModelEffectHandler() const noexcept
{
	return m_Impl->GetModelEffectHandler();
}
LightHandler& Engine::GetLightHandler() const noexcept
{
	return m_Impl->GetLightHandler();
}
PhysicsHandler& Engine::GetPhysicsHandler() const noexcept
{
	return m_Impl->GetPhysicsHandler();
}
SoundEngine& Engine::GetSoundEngine() const noexcept
{
	return m_Impl->GetSoundEngine();
}
BufferHandler& Engine::GetBufferHandler() const noexcept
{
	return m_Impl->GetBufferHandler();
}
TextureRenderingHandler& Engine::GetTextureRenderingHandler() const noexcept
{
	return m_Impl->GetTextureRenderingHandler();
}
const float Engine::Impl::GetFPS() noexcept
{
	return m_frameTimer.GetRealFrameRate();
}
const float Engine::Impl::GetDeltaTime() noexcept
{
	return m_frameTimer.GetRealDeltaTime();
}
SceneManager& Engine::Impl::GetSceneManager() noexcept
{
	return m_sceneManager;
}
ModelHandler& Engine::Impl::GetModelFactory() noexcept
{
	return m_modelHandler;
}
SpriteHandler& Engine::Impl::GetSpriteFactory() noexcept
{
	return m_spriteHandler;
}
RenderManager& Engine::Impl::GetRenderManager() noexcept
{
	return m_renderManager;
}
D3D12Framework& Engine::Impl::GetFramework() noexcept
{
	return m_framework;
}

TextureHandler& Engine::Impl::GetTextureHandler() noexcept
{
	return m_textureHandler;
}

ModelEffectHandler& Engine::Impl::GetModelEffectHandler() noexcept
{
	return m_modelEffectHandler;
}

LightHandler& Engine::Impl::GetLightHandler() noexcept
{
	return m_lightHandler;
}

PhysicsHandler& Engine::Impl::GetPhysicsHandler() noexcept
{
	return m_physicsHandler;
}

SoundEngine& Engine::Impl::GetSoundEngine() noexcept
{
	return m_soundEngine;
}

BufferHandler& Engine::Impl::GetBufferHandler() noexcept
{
	return m_bufferHandler;
}

TextureRenderingHandler& Engine::Impl::GetTextureRenderingHandler() noexcept
{
	return m_renderManager.GetTextureRendering();
}
