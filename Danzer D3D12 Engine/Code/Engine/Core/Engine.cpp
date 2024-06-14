#include "stdafx.h"
#include "Engine.h"
#include "Level Loader/LevelLoaderCustom.h"

#include "Components/Transform.h"
#include "Core/input.hpp"
#include "Rendering/TextureHandler.h"
#include "DirectX12Framework.h"
#include "WindowHandler.h"
#include "SceneManager.h"
#include "Rendering/RenderManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/Models/ModelEffectHandler.h"
#include "Rendering/Screen Rendering/LightHandler.h"
#include "Scene.h"
#include "FrameTimer.h"
#include "Rendering/2D/SpriteHandler.h"
#include "Rendering/SkyBox.h"
#include "CollisionManager.h"
#include "Rendering/Camera.h"
#include "D3D12Framework.h"
//#include "PhysicsWrapper.h"

// ImGui
#include "imgui-master/backends/imgui_impl_dx12.h"
#include "imgui-master/backends/imgui_impl_win32.h"


class Engine::Impl {
public:
	Impl(unsigned int width, unsigned int height);
	~Impl();

	void BeginUpdate();
	void MidUpdate();
	void LateUpdate();

	void EndInitFrame();

	const float			GetFPS()			    noexcept;
	const float		    GetDeltaTime()		    noexcept;
	SceneManager&		GetSceneManager()	    noexcept;
	ModelHandler&		GetModelFactory()	    noexcept;
	SpriteHandler&		GetSpriteFactory()	    noexcept;
	RenderManager&	    GetRenderManager()	    noexcept;
	D3D12Framework&     GetFramework()		    noexcept;
	TextureHandler&		GetTextureHandler()	    noexcept;
	CollisionManager&   GetCollisionManager()   noexcept;
	ModelEffectHandler& GetModelEffectHandler() noexcept;
	LightHandler&		GetLightHandler()	    noexcept;

private:
	WindowHandler	   m_windowHandler;
	D3D12Framework	   m_framework;
	TextureHandler	   m_textureHandler;
	RenderManager      m_renderManager;
	ModelHandler	   m_modelHandler;
	SpriteHandler	   m_spriteHandler;
	SceneManager	   m_sceneManager;
	//PhysicsWrapper	   m_physicsManager;
	CollisionManager   m_collisionManager;
	ModelEffectHandler m_modelEffectHandler;
	LightHandler	   m_lightHandler;
	FrameTimer		   m_frameTimer;
	Skybox			   m_skybox;	
	Camera			   m_camera;

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
	m_lightHandler(m_framework),
	m_sceneManager(),
	//m_physicsManager(10240, 0, 65536, 20480),
	m_collisionManager(),
	m_camera(65.f, (float)m_windowHandler.WindowData().m_w / (float)m_windowHandler.WindowData().m_h),
	m_skybox(m_textureHandler),
	m_deltaTime(0.f)
{
	//JPH::PhysicsSystem physicsSystem;
	//physicsSystem.Init(1000, 0, 1000, 1000, )
	FMOD_RESULT fResult;
	FMOD::System* fmodSystem = nullptr;
	FMOD::System_Create(&fmodSystem);
	fResult = fmodSystem->init(256, FMOD_INIT_3D_RIGHTHANDED, nullptr);
	if (fResult != FMOD_OK)
		throw fResult;
	

	ImGuiIO* io = &ImGui::GetIO();
	ImVec2 vec;
	vec.x = (float)width;
	vec.y = (float)height;

	io->DisplaySize = vec;
	io->Fonts->Build();

	CustomModel skyboxCube = ModelData::GetCube();
	skyboxCube.m_customModelName = "skybox";
	m_skybox.Init(m_renderManager.GetPSOHandler(), m_modelHandler.CreateCustomModel(skyboxCube).m_modelID, L"Sprites/defaultRedSkybox.dds", true);
	m_spriteHandler.CreateSpriteSheet(L"Sprites/testSpriteSheet.dds", 4, 4);
	
	m_sceneManager.Init(m_camera);
}

Engine::Impl::~Impl()
{
	m_framework.~D3D12Framework();
	m_sceneManager.~SceneManager();
	m_renderManager.~RenderManager();
	m_windowHandler.~WindowHandler();
	m_modelHandler.~ModelHandler();
	m_spriteHandler.~SpriteHandler();
	m_collisionManager.~CollisionManager();
	m_skybox.~Skybox();
}

Engine::Engine() : m_Impl(nullptr){}
Engine::Engine(unsigned int width, unsigned int height)
{
	m_Impl = new Engine::Impl(width, height);
}
Engine::~Engine(){}

void Engine::Impl::BeginUpdate()
{
	m_frameTimer.Update();
	const float deltaTime = m_frameTimer.GetRealDeltaTime();

	m_renderManager.BeginFrame();
}

void Engine::Impl::MidUpdate()
{
	const float deltaTime = m_frameTimer.GetRealDeltaTime();
	m_skybox.Update(deltaTime);

	m_sceneManager.GetCurrentScene().UpdateTransforms();
	m_renderManager.RenderFrame(m_lightHandler, m_textureHandler, m_modelHandler, m_modelEffectHandler, m_spriteHandler, m_skybox, m_sceneManager.GetCurrentScene());
}

void Engine::Impl::LateUpdate()
{
	m_framework.ExecuteCommandList();
	m_framework.GetSwapChain()->Present(1, 0);
	m_framework.WaitForGPU();
}

void Engine::Impl::EndInitFrame()
{
	m_framework.EndInitFrame();
}

bool Engine::StartEngine(bool editor)
{
	if (editor)
		s_engineState = EngineState::ENGINE_STATE_EDITOR;
	else
		s_engineState = EngineState::ENGINE_STATE_GAME;
	
	return true;
}

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
//LevelLoaderCustom& Engine::GetLevelLoader() const noexcept
//{
//	return m_Impl->GetLevelLoader();
//}
TextureHandler& Engine::GetTextureHandler() const noexcept
{
	return m_Impl->GetTextureHandler();
}
CollisionManager& Engine::GetCollisionManager() const noexcept
{
	return m_Impl->GetCollisionManager();
}
ModelEffectHandler& Engine::GetModelEffectHandler() const noexcept
{
	return m_Impl->GetModelEffectHandler();
}
LightHandler& Engine::GetLightHandler() const noexcept
{
	return m_Impl->GetLightHandler();
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
//LevelLoaderCustom& Engine::Impl::GetLevelLoader() noexcept
//{
//	return m_levelLoader;
//}
TextureHandler& Engine::Impl::GetTextureHandler() noexcept
{
	return m_textureHandler;
}
CollisionManager& Engine::Impl::GetCollisionManager() noexcept
{
	return m_collisionManager;
}

ModelEffectHandler& Engine::Impl::GetModelEffectHandler() noexcept
{
	return m_modelEffectHandler;
}

LightHandler& Engine::Impl::GetLightHandler() noexcept
{
	return m_lightHandler;
}
