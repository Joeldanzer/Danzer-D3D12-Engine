#include "Game.h"
#include "Engine/Core/Engine.h"

#include "Engine/Core/input.hpp"

#include "SceneManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"
#include "Rendering/2D/SpriteHandler.h"

#include "Components/Transform2D.h"
#include "Components/Model.h"
#include "Components/Transform.h"
#include "Components/DirectionalLight.h"
#include "Components/Sprite.h"


class Game::Impl {
public:
	Impl(Engine& engine);
	~Impl();

	void Update(const float dt);

private:
	Engine& m_engine;

	float m_currentTime;
	float m_time = 0.2f;
	entt::entity m_entity;
};

Game::Impl::Impl(Engine& engine) :
	m_engine(engine)
{
	entt::registry& reg = engine.GetSceneManager().GetCurrentScene().Registry();

	engine.GetSpriteHandler().CreateSpriteSheet(L"Sprites/RunTestAnimation.dds", 8, 8);

	m_entity = reg.create();
	Transform2D& transform = reg.emplace<Transform2D>(m_entity);
	Object& obj			   = reg.emplace<Object>(m_entity);
	Sprite& sprite		   = reg.emplace<Sprite>(m_entity);

	obj.m_state = Object::STATE::ACTIVE;
	//sprite.m_spriteSheet = engine.GetSpriteHandler().GetCreatedSpriteSheet("testSpriteSheet");
	sprite.m_spriteSheet = engine.GetSpriteHandler().GetCreatedSpriteSheet("testSpriteSheet");
	transform.m_scale    = { 0.2f, 0.2f };
	transform.m_position = {-0.9f, 0.8f };

	m_currentTime = m_time;
	
	auto entity = reg.create();
	reg.emplace<Transform>(entity);
	reg.emplace<Object>(entity);
    reg.emplace<Model>(entity, engine.GetModelHandler().LoadModel(L"Models/Particle_Chest.fbx", "Spinza Atrium"));
}
Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();

	Sprite& sprite = reg.get<Sprite>(m_entity);
	m_currentTime -= dt;
	if (m_currentTime < 0.f) {
		sprite.m_frame++;
		if (sprite.m_frame > 15)
			sprite.m_frame = 0;

		m_currentTime = m_time;
	}

	Transform& transform = reg.get<Transform>(m_engine.GetSceneManager().GetCurrentScene().GetMainCamera());
		
	if (Input::GetInstance().IsKeyDown(VK_RIGHT))
		transform.m_rotation *= Quat4f::CreateFromAxisAngle(Vect3f().Up,  dt * 2.f);
	if (Input::GetInstance().IsKeyDown(VK_LEFT))
		transform.m_rotation *= Quat4f::CreateFromAxisAngle(Vect3f().Up, -(dt * 2.f));
	
	if (Input::GetInstance().IsKeyDown(VK_UP))
		transform.m_rotation *= Quat4f::CreateFromAxisAngle(transform.World().Left(), dt * 2.f);
	if (Input::GetInstance().IsKeyDown(VK_DOWN))
		transform.m_rotation *= Quat4f::CreateFromAxisAngle(transform.World().Left(), -(dt * 2.f));

	if (Input::GetInstance().IsKeyDown('W'))
		transform.m_position.z += dt * 5.0f;
	if (Input::GetInstance().IsKeyDown('S'))
		transform.m_position.z -= dt * 5.0f;
	if (Input::GetInstance().IsKeyDown('A'))
		transform.m_position.x += dt * 5.0f;
	if (Input::GetInstance().IsKeyDown('D'))
		transform.m_position.x -= dt * 5.0f;

	if (Input::GetInstance().IsKeyDown(VK_SPACE))
		transform.m_position.y += dt * 5.0f;
	if (Input::GetInstance().IsKeyDown(VK_SHIFT))
		transform.m_position.y -= dt * 5.0f;

	if (Input::GetInstance().IsKeyPressed('Z')) {
		auto cameraList = reg.view<Camera>();
		for (UINT i = 0; i < cameraList.size(); i++)
		{
			Camera& cam = reg.get<Camera>(cameraList[i]);
			cam.RenderTarget() = cam.RenderTarget() < 7 ? cam.RenderTarget() + 1 : 0;
		}
	}

}

Game::Game(Engine& engine) :
	m_impl(new Impl(engine)){}
Game::~Game(){
	m_impl->~Impl();
	delete m_impl;
	m_impl = nullptr;
}
void Game::Update(const float deltaTime){m_impl->Update(deltaTime);}
