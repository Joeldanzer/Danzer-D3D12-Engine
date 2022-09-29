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
	entt::registry& reg = engine.SceneManager().GetCurrentScene().Registry();

	entt::entity copper  = engine.SceneManager().GetCurrentScene().CreateBasicEntity("ParticleChest");

	Model copperSphere = engine.ModelHandler().LoadModel(L"Models/VertexPaintPlane.fbx", "Chest");
	reg.emplace<Model>(copper, copperSphere);
	
	std::array<std::string, 3> textures;
	textures[0] = "Sprites/Copper_01_a.dds";
	textures[1] = "Sprites/Copper_01_n.dds";
	textures[2] = "Sprites/Copper_01_m.dds";
	float color[4] = { 1.f, 1.f, 1.f, 1.f };
	Material material = m_engine.TextureHandler().CreateMaterial(textures.data(), 1.f, 1.f, 1.f, color);
	m_engine.ModelHandler().SetMaterialForModel(copperSphere.m_modelID, material);
}
Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	entt::registry& reg = m_engine.SceneManager().GetCurrentScene().Registry();
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
