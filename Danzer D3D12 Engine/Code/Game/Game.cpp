#include "Game.h"
#include "Engine/Core/Engine.h"

#include "Rendering/Models/ModelEffectHandler.h"

#include "Engine/Core/input.hpp"

#include "Rendering/Camera.h"
#include "SceneManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"
#include "Rendering/2D/SpriteHandler.h"
#include "Rendering/Screen Rendering/LightHandler.h"
#include "Components/Render & Effects/WaterPlaneBufferData.h"
#include "Sound/SoundHeader.h"

#include "Physics/PhysicsHeader.h"

#include "Components/Light/PointLight.h"
#include "Components/2D/Transform2D.h"
#include "Components/Model.h"
#include "Components/Transform.h"
#include "Components/Light/DirectionalLight.h"
#include "Components/2D/Sprite.h"
#include "Components/Sound/SoundListener.h"
#include "Components/Sound/SoundSource.h"

class Game::Impl {
public:
	Impl(Engine& engine);
	~Impl();

	void Update(const float dt);

	
private:
	//void OnSphere(GameEntity& collidedEntity);
	void OnSphereContact(GameEntity& collidedEntity);

	Engine& m_engine;

	PhysicsBody* m_sphere = nullptr;
	GameEntity* enttTest;

	SOUND_ID pipeSound;

	//FMOD::Sound* m_pipeSound = nullptr;

	float m_currentTime;
	float m_time = 0.2f;
	entt::entity m_entity;
};

Game::Impl::Impl(Engine& engine) :
	m_engine(engine)
{
	entt::registry& reg = engine.GetSceneManager().Registry();

	//engine.GetSpriteHandler().CreateSpriteSheet(L"Sprites/RunTestAnimation.dds", 8, 8);
	//
	//m_entity = reg.create();
	//Transform2D& transform = reg.emplace<Transform2D>(m_entity);
	//Object& obj			   = reg.emplace<Object>(m_entity);
	//Sprite& sprite		   = reg.emplace<Sprite>(m_entity);
	
	//obj.m_state = Object::STATE::ACTIVE;
	//sprite.m_spriteSheet = engine.GetSpriteHandler().GetCreatedSpriteSheet("testSpriteSheet");
	//transform.m_scale    = { 0.2f, 0.2f };
	//transform.m_position = {-0.9f, 0.8f };

	m_currentTime = m_time;
	
	entt::entity entity = reg.create();

	Transform& modelTransform = reg.emplace<Transform>(entity);
	modelTransform.m_scale	  = { 1.0f, 1.0f, 1.0f };
	modelTransform.m_position = { 0.0f, 0.0f, 0.0f };
	modelTransform.m_rotation = Quat4f::CreateFromAxisAngle(Vect3f::Up, ToRadians(180.0f));
	GameEntity& sponzaObj = reg.emplace<GameEntity>(entity, entity);
	sponzaObj.m_name = "Sponza Atrium";

	//m_engine.GetModelHandler().LoadModelsToScene(reg, L"Models/BlenderSponzaAtriumNew.fbx");

    reg.emplace<Model>(entity, engine.GetModelHandler().LoadModel(L"Models/BlenderSponzaAtriumOld.fbx", "Sponza Atrium"));
	
	//engine.GetSoundEngine().CreateSound("Sound/MetalPipe.wav", FMOD_DEFAULT, nullptr, &m_pipeSound);
	//pipeSound = engine.GetSoundEngine().LoadSound("Sound/MetalPipe.wav", false);
	//
	//auto camEntt = engine.GetSceneManager().GetMainCamera();
	//engine.GetSoundEngine().CreateSoundListener(reg.emplace<SoundListener>(camEntt));
	//
	//enttTest = &engine.GetSceneManager().CreateBasicEntity("Physics Sphere", false);
	//m_sphere = &reg.emplace<PhysicsBody>(enttTest->m_entity, engine.GetPhysicsHandler().CreatePhyscisSphere(*enttTest, 1.0f, EMotionType::Dynamic, EActivation::DontActivate, Layers::MOVING));
	//reg.emplace<SoundSource>(enttTest->m_entity);
	//m_sphere->OnContactAdded = std::bind(&Game::Impl::OnSphereContact, this, std::placeholders::_1);

	//reg.emplace<Model>(enttTest->m_entity, engine.GetModelHandler().LoadModel(L"Models/sphere.fbx"));
	//Transform& sphereT = reg.get<Transform>(enttTest->m_entity);
	//sphereT.m_position = { 0.0f, 10.0f, 0.0f };


	//GameEntity& staticbox = engine.GetSceneManager().CreateBasicEntity("Static Box", true);
	//reg.emplace<Model>(staticbox.m_entity, engine.GetModelHandler().LoadModel(L"Models/cube.fbx"));
	//reg.emplace<PhysicsBody>(staticbox.m_entity, engine.GetPhysicsHandler().CreatePhysicsBox(staticbox, {20.0f, 0.2f, 20.0f}, EMotionType::Static, EActivation::Activate, Layers::NON_MOVING));

	//Transform& boxT = reg.get<Transform>(staticbox.m_entity);
	//boxT.m_scale = { 20.0f, 0.2f, 20.0f };
	//boxT.m_rotation = Quat4f::CreateFromAxisAngle(Vect3f::Right, ToRadians(0.0f));
	//auto waterPlane = engine.GetSceneManager().GetCurrentScene().CreateBasicEntity("WaterPlane");
	//Model waterModel = reg.emplace<Model>(waterPlane, engine.GetModelHandler().LoadModel(L"Models/WaterPlane.fbx", "Water Plane"));
	//std::vector<UINT> textures = {
	//	engine.GetTextureHandler().GetTexture(L"Sprites/WaterNoiseVertex.dds"),
	//	engine.GetTextureHandler().GetTexture(L"Sprites/WaterNoiseNormalOne.dds"),
	//	engine.GetTextureHandler().GetTexture(L"Sprites/WaterNoiseNormalTwo.dds")
	//};
	//
	//WaterPlaneData waterData;
	//waterData.m_waterColorOne	  = { 0.1f, 0.5f, 0.95f };
	//waterData.m_waterColorTwo	  = { 0.2f, 0.7f, 1.0f };
	//waterData.m_roughness		  = 1.0f;
	//waterData.m_metallic		  = 0.1f;
	//waterData.m_waterDirectionOne = { 1.0f,  0.2f };
	//waterData.m_waterDirectionTwo = {-0.5f, -1.0f };
	//waterData.m_speed			  = 0.01f;
	//waterData.m_textureScale	  = 5.0f;
	//waterData.m_noiseScale		  = 100.f;
	//waterData.m_heightScale		  = 0.75f;
	//waterData.m_near			  = 0.1f;
	//waterData.m_far				  = 1000.0f;
	//waterData.m_edgeScale	      = 1.0f;
	//waterData.m_edgeColor	      = { 1.0f, 1.0f, 1.0f };

	//reg.emplace<ModelEffect>(waterPlane, engine.GetModelEffectHandler().CreateModelEffect({L"Shaders/WaterPlaneVS.cso", L"Shaders/WaterPlanePS.cso"}, waterModel.m_modelID, &waterData, sizeof(WaterPlaneData), textures, true));
	//Transform& waterTransform = reg.get<Transform>(waterPlane);
	//waterTransform.m_position = { 0.0f, 2.0f, 0.0f };
	//waterTransform.m_scale	  = { 1.0f, 1.0f, 1.5f };

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
	entt::registry& reg = m_engine.GetSceneManager().Registry();
	Transform& transform = reg.get<Transform>(m_engine.GetSceneManager().GetMainCamera());
	
	float speed = 5.0f;

	if (Input::GetInstance().IsKeyDown(VK_LEFT))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f::Up,  dt * 2.f);
	if (Input::GetInstance().IsKeyDown(VK_RIGHT))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f::Up, -(dt * 2.f));
	
	if (Input::GetInstance().IsKeyDown(VK_DOWN))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(transform.World().Right(), -(dt * 2.f));
	if (Input::GetInstance().IsKeyDown(VK_UP))
		transform.m_rotation *= DirectX::XMQuaternionRotationAxis(transform.World().Right(), dt * 2.f);

	Vector3 forward = transform.World().Forward();

	if (Input::GetInstance().IsKeyDown('W'))
		transform.m_position += (transform.World().Forward() * speed) * dt;
	if (Input::GetInstance().IsKeyDown('S'))
		transform.m_position -= (transform.World().Forward() * speed) * dt;

	if (Input::GetInstance().IsKeyDown('A'))
		transform.m_position -= (transform.World().Right() * speed) * dt;
	if (Input::GetInstance().IsKeyDown('D'))
		transform.m_position += (transform.World().Right() * speed) * dt;

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

	if (Input::GetInstance().IsKeyPressed('P'))
		reg.get<PhysicsBody>(enttTest->m_entity).ActivateBody();
}

void Game::Impl::OnSphereContact(GameEntity& collidedEntity)
{
	//std::cout << "Sphere will do something!" << std::endl;
	PhysicsBody& body = m_engine.GetSceneManager().Registry().get<PhysicsBody>(enttTest->m_entity);
	body.AddLinearVelocity({ 0.0f, 15.0f, 0.0f });

	m_engine.GetSoundEngine().PlaySoundAtEntt(enttTest->m_entity, pipeSound);
	//m_engine.GetSoundEngine().PlayFSound(m_pipeSound);
}

Game::Game(Engine& engine) :
	m_impl(new Impl(engine)){}
Game::~Game(){
	m_impl->~Impl();
	delete m_impl;
	m_impl = nullptr;
}
void Game::Update(const float deltaTime){m_impl->Update(deltaTime);}
