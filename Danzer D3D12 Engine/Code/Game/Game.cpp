#include "Game.h"
#include "Engine/Core/Engine.h"

#include "SceneManager.h"
#include "Rendering/Models/ModelHandler.h"

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

	entt::entity chest = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("PBRChest");
	Model model = engine.GetModelHandler().LoadModel(L"Models/Particle_Chest.fbx");
	reg.emplace<Model>(chest, model);

	engine.GetModelHandler().SetAlbedoForModel(model.m_modelID, { "Sprites/Particle_Chest_A.dds" });
	engine.GetModelHandler().SetNormalForModel(model.m_modelID, { "Sprites/Particle_Chest_N.dds" });
	engine.GetModelHandler().SetMaterialForModel(model.m_modelID, { "Sprites/Particle_Chest_M.dds" });

}
Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();

	auto view = reg.view<Transform, DirectionalLight>();

	for (auto i : view )
	{
		Transform& transform = reg.get<Transform>(i);

		transform.m_rotation *= Quat4f::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, dt);
		int s = 2;
	}

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
