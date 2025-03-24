#include "GamePCH.h"

#include "Game.h"

#include "Engine/Core/Engine.h"

#include "Rendering/Models/ModelEffectHandler.h"

#include "Engine/Core/input.hpp"

#include "Rendering/Camera.h"
#include "SceneManager.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"
#include "Rendering/2D/SpriteHandler.h"
#include "Rendering/Data/LightHandler.h"
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

#include "Components/AllComponents.h"

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

	entt::entity m_frustrumTest;
};

Game::Impl::Impl(Engine& engine) :
	m_engine(engine)
{
	//std::strivb jn          ng name = instance.GetComponentName();

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
	
	Entity entity = Reg::Instance()->Create3DEntity("Sponzra Atrium");

	Transform& modelTransform = Reg::Instance()->Get<Transform>(entity);
	modelTransform.m_scale	  = { 1.0f, 1.0f, 1.0f };
	modelTransform.m_position = { 0.0f, 0.0f, 0.0f };
	modelTransform.m_rotation = Quat4f::CreateFromAxisAngle(Vect3f::Up, ToRadians(180.0f));
	
    REGISTRY->Emplace<Model>(entity, engine.GetModelHandler().LoadModel(L"Models/BlenderSponzaAtriumNew.fbx", "Sponza Atrium"));
	Model& model = REGISTRY->Get<Model>(entity);
	int s = 1;
	//for (int32_t x = -5; x < 5 + 1; x++)
	//{
	//	for (int32_t z = -5; z < 5 + 1; z++)
	//	{	
	//		GameEntity& entity = engine.GetSceneManager().CreateBasicEntity("FrustrumTest", false);
	//		reg.emplace<Model>(entity.m_entity, engine.GetModelHandler().LoadModel(L"Models/Particle_Chest.fbx", "ParticleChest"));
	//
	//		Transform& transform = reg.get<Transform>(entity.m_entity);
	//		transform.m_position = { (float)x * 5, 0.0f, (float)z* 5};
	//		transform.m_scale = { 1.0f, 1.0f, 1.0f };
	//		
	//	}
	//}

	//m_frustrumTest = engine.GetSceneManager().CreateBasicEntity("FrustrumTest", false).m_entity;
	//Camera& cam = reg.get<Camera>(engine.GetSceneManager().GetMainCamera());
	//cam.SetFrustrumTest(&reg.get<Transform>(m_frustrumTest));
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

}	

Game::Impl::~Impl(){}

void Game::Impl::Update(const float dt)
{
	Transform& transform = Reg::Instance()->Get<Transform>(m_engine.GetSceneManager().GetMainCamera());
	
	//Transform& frustrumTransform = reg.get<Transform>(m_frustrumTest);
	//if (Input::GetInstance().IsKeyDown('Q')) {
	//	frustrumTransform.m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f::Up, dt * -2.0f);
	//}
	//if (Input::GetInstance().IsKeyDown('E')) {
	//	frustrumTransform.m_rotation *= DirectX::XMQuaternionRotationAxis(Vect3f::Up, dt * 2.0f);
	//}

	//if (Input::GetInstance().IsKeyDown('C')) {
	//	frustrumTransform.m_position.z += 8.0f * dt;
	//}
	//if (Input::GetInstance().IsKeyDown('X')) {
	//	frustrumTransform.m_position.z -= 8.0f * dt;
	//}
	//if (Input::GetInstance().IsKeyDown('B')) {
	//	frustrumTransform.m_position.x += 8.0f * dt;
	//}
	//if (Input::GetInstance().IsKeyDown('N')) {
	//	frustrumTransform.m_position.x -= 8.0f * dt;
	// 
	//}

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
		auto cameraList = Reg::Instance()->GetRegistry().view<Camera>();
		for (auto ent : cameraList)
		{
			Camera& cam = Reg::Instance()->Get<Camera>(ent);
			cam.RenderTarget() = cam.RenderTarget() < 9 ? cam.RenderTarget() + 1 : 0;
		}
	}
}

// Testing for custom functions for Collision Detection.
void Game::Impl::OnSphereContact(GameEntity& collidedEntity)
{
	//std::cout << "Sphere will do something!" << std::endl;
	//PhysicsBody& body = m_engine.GetSceneManager().Registry().get<PhysicsBody>(enttTest->m_entity);
	//body.AddLinearVelocity({ 0.0f, 15.0f, 0.0f });
	//
	//m_engine.GetSoundEngine().PlaySoundAtEntt(enttTest->m_entity, pipeSound);
}

Game::Game(Engine& engine) :
	m_impl(new Impl(engine)){}
Game::~Game(){
	m_impl->~Impl();
	delete m_impl;
	m_impl = nullptr;
}
void Game::Update(const float deltaTime){m_impl->Update(deltaTime);}
