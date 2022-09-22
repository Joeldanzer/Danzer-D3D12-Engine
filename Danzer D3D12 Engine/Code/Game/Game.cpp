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

	entt::entity crate = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("Crate");
	entt::entity containerBlue  = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("ContainerBlue");
	//entt::entity containerRed   = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("ContainerRed");
	//entt::entity containerGreen = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("ContainerGreen");

	Model crateModel = engine.GetModelHandler().LoadModel(L"Models/WoodenCrate.fbx");
	reg.emplace<Model>(crate, crateModel);

	Model container = engine.GetModelHandler().LoadModel(L"Models/VertexPaintPlane.fbx");
	reg.emplace<Model>(containerBlue, container);

	engine.GetModelHandler().SetAlbedoForModel(crateModel.m_modelID, { "Sprites/WoodenCrate_A.dds" });
	engine.GetModelHandler().SetNormalForModel(crateModel.m_modelID, { "Sprites/WoodenCrate_N.dds" });
	engine.GetModelHandler().SetMaterialValues(crateModel.m_modelID, 0.0f, 0.25f, 10.f);

	engine.GetModelHandler().SetAlbedoForModel(container.m_modelID,  { "Sprites/Copper_01_a.dds" });
	engine.GetModelHandler().SetNormalForModel(container.m_modelID,  { "Sprites/Copper_01_n.dds" });
	engine.GetModelHandler().SetMaterialForModel(container.m_modelID,{ "Sprites/Copper_01_m.dds" });
	//engine.GetModelHandler().SetMaterialValues(container.m_modelID, 1.0f, 0.25f, 0.f);

	reg.get<Transform>(containerBlue).m_position = { 5.f, 0.f, 0.f };
	//engine.GetModelHandler().SetMaterialForModel(model.m_modelID, { "Sprites/Milk_Churn_M.dds" });

}
Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();

	auto view = reg.view<Transform, DirectionalLight>();

	for (auto i : view )
	{
		Transform& transform = reg.get<Transform>(i);

		//transform.m_rotation *= Quat4f::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, dt);
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
