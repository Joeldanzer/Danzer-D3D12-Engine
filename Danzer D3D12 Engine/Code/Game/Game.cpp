#include "Game.h"
#include "Engine/Core/Engine.h"

#include "SceneManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"

#include "Components/Model.h"
#include "Components/Transform.h"
#include "Components/DirectionalLight.h"


class Game::Impl {
public:
	Impl(Engine& engine);
	~Impl();

	void Update(const float dt);

private:
	Engine& m_engine;
};

Game::Impl::Impl(Engine& engine) :
	m_engine(engine)
{
	entt::registry& reg = engine.GetSceneManager().GetCurrentScene().Registry();

	entt::entity copper  = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("ContainerBlue");

	Model copperSphere = engine.GetModelHandler().LoadModel(L"Models/SphereTest.fbx", "CopperSphere");
	reg.emplace<Model>(copper, copperSphere);
	
	std::array<std::string, 3> textures;
	textures[0] = "Sprites/BlueRoof_A.dds";
	textures[1] = "Sprites/BlueRoof_N.dds";
	textures[2] = "Sprites/BlueRoof_M.dds";
	float color[4] = { 1.f, 1.f, 1.f, 1.f };
	Material material = m_engine.GetTextureHandler().CreateMaterial(textures.data(), 1.f, 1.f, 0.f, color);
	m_engine.GetModelHandler().SetMaterialForModel(copperSphere.m_modelID, material);
}
Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();
}


// DONT TOUCH!!!
Game::Game(Engine& engine) :
	m_impl(new Impl(engine)){}
Game::~Game(){
	m_impl->~Impl();
	delete m_impl;
	m_impl = nullptr;
}
void Game::Update(const float deltaTime){m_impl->Update(deltaTime);}
