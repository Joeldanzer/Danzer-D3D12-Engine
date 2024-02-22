#include "Game.h"
#include "Engine/Core/Engine.h"

#include "Rendering/Models/ModelEffectHandler.h"

#include "Engine/Core/input.hpp"

#include "SceneManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"
#include "Rendering/2D/SpriteHandler.h"
#include "Rendering/Screen Rendering/LightHandler.h"
#include "Components/WaterPlaneBufferData.h"

#include "Components/PointLight.h"
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

	//engine.GetSpriteHandler().CreateSpriteSheet(L"Sprites/RunTestAnimation.dds", 8, 8);
	//
	//m_entity = reg.create();
	//Transform2D& transform = reg.emplace<Transform2D>(m_entity);
	//Object& obj			   = reg.emplace<Object>(m_entity);
	//Sprite& sprite		   = reg.emplace<Sprite>(m_entity);
	//
	//obj.m_state = Object::STATE::ACTIVE;
	//sprite.m_spriteSheet = engine.GetSpriteHandler().GetCreatedSpriteSheet("testSpriteSheet");
	//transform.m_scale    = { 0.2f, 0.2f };
	//transform.m_position = {-0.9f, 0.8f };

	m_currentTime = m_time;
	
	auto entity = reg.create();
	Transform& modelTransform = reg.emplace<Transform>(entity);
	modelTransform.m_scale	  = {1.0f, 1.0f, 1.0f};
	modelTransform.m_position = { 0.0f, 0.0f, 0.0f };
	Object& sponzaObj = reg.emplace<Object>(entity);
	sponzaObj.m_name = "Sponza Atrium";

    reg.emplace<Model>(entity, engine.GetModelHandler().LoadModel(L"Models/BlenderSponzaAtriumOld.fbx", "Sponza Atrium"));
	
	auto waterPlane = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("WaterPlane");
	Model waterModel = reg.emplace<Model>(waterPlane, engine.GetModelHandler().LoadModel(L"Models/WaterPlane.fbx", "Water Plane"));
	std::vector<UINT> textures = {
		engine.GetTextureHandler().GetTexture(L"Sprites/WaterNoiseVertex.dds"),
		engine.GetTextureHandler().GetTexture(L"Sprites/WaterNoiseNormalOne.dds"),
		engine.GetTextureHandler().GetTexture(L"Sprites/WaterNoiseNormalTwo.dds")
	};

	WaterPlaneData waterData;
	waterData.m_waterColorOne	  = { 0.1f, 0.5f, 0.95f };
	waterData.m_waterColorTwo	  = { 0.2f, 0.7f, 1.0f };
	waterData.m_roughness		  = 0.1f;
	waterData.m_metallic		  = 0.1f;
	waterData.m_waterDirectionOne = { 1.0f,  0.2f };
	waterData.m_waterDirectionTwo = {-0.5f, -1.0f };
	waterData.m_speed			  = 0.01f;
	waterData.m_textureScale	  = 5.0f;
	waterData.m_noiseScale		  = 100.f;
	waterData.m_heightScale		  = 0.75f;
	waterData.m_near			  = 0.1f;
	waterData.m_far				  = 1000.0f;
	waterData.m_edgeScale	      = 1.0f;
	waterData.m_edgeColor	      = { 1.0f, 1.0f, 1.0f };
	reg.emplace<ModelEffect>(waterPlane, engine.GetModelEffectHandler().CreateModelEffect(L"WaterPlane", waterModel.m_modelID, &waterData, sizeof(WaterPlaneData), textures, true));
	Transform& waterTransform = reg.get<Transform>(waterPlane);
	waterTransform.m_position = { 0.0f, 2.0f, 0.0f };
	waterTransform.m_scale	  = { 1.0f, 1.0f, 1.5f };

	//for (UINT i = 1; i < 12; i++)
	//{
	//	entt::entity point = m_engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("PointLight");
	//	PointLight& light = reg.emplace<PointLight>(point, m_engine.GetLightHandler().CreatePointLight());
	//	light.m_color = { 1.0f - (i/10.0f), 0.0f, (i /10.f), 1.0f };
	//	light.m_range = 1.0f;
	//	
	//	Transform& lightTransform = reg.get<Transform>(point);
	//	lightTransform.m_position = { 0.0f, 5.0f, -5.0f + i };
	//}
}	

Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	entt::registry& reg = m_engine.GetSceneManager().GetCurrentScene().Registry();
	Transform& transform = reg.get<Transform>(m_engine.GetSceneManager().GetCurrentScene().GetMainCamera());
	
	float speed = 1.0f;

	if (Input::GetInstance().IsKeyDown(VK_RIGHT))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f().Up,  dt * 2.f);
	if (Input::GetInstance().IsKeyDown(VK_LEFT))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f().Up, -(dt * 2.f));
	
	if (Input::GetInstance().IsKeyDown(VK_UP))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(transform.World().Left(), dt * 2.f);
	if (Input::GetInstance().IsKeyDown(VK_DOWN))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(transform.World().Left(), -(dt * 2.f));

	if (Input::GetInstance().IsKeyDown('W'))
		transform.m_position.z += dt * speed;
	if (Input::GetInstance().IsKeyDown('S'))
		transform.m_position.z -= dt * speed;
	if (Input::GetInstance().IsKeyDown('A'))
		transform.m_position.x -= dt * speed;
	if (Input::GetInstance().IsKeyDown('D'))
		transform.m_position.x += dt * speed;

	if (Input::GetInstance().IsKeyDown(VK_SPACE))
		transform.m_position.y += dt * speed;
	if (Input::GetInstance().IsKeyDown(VK_SHIFT))
		transform.m_position.y -= dt * speed;

	if (Input::GetInstance().IsKeyPressed('Z')) {
		auto cameraList = reg.view<Camera>();
		for (UINT i = 0; i < cameraList.size(); i++)
		{
			Camera& cam = reg.get<Camera>(cameraList[i]);
			cam.RenderTarget() = cam.RenderTarget() < 9 ? cam.RenderTarget() + 1 : 0;
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
