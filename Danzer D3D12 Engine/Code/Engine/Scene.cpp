#include "stdafx.h"
#include "Scene.h"

#include "Components/GameEntity.h"
#include "Components/Transform.h"

#include <string>

Scene::Scene(){}
Scene::~Scene(){}
GameEntity& Scene::CreateBasicEntity(std::string name, bool isStatic, GameEntity::STATE state)
{
	auto entity = m_sceneRegistry.create();

	GameEntity obj(entity);
	obj.m_name = name;
	//obj.m_layer = layer;
	//obj.m_tag = tag;
	obj.m_static = isStatic;
	obj.m_state = state;
	m_sceneRegistry.emplace<GameEntity>(entity, obj);

	Transform transform;
	m_sceneRegistry.emplace<Transform>(entity, transform);

	return m_sceneRegistry.get<GameEntity>(entity);
}

//void Scene::SetMainCamera(Camera& camera)
//{
//	m_mainCamera = camera;
//}

//void Scene::AddObject(Object* object) {
//	if (object) {
//		for (UINT i = 0; i < m_objects.size(); i++)
//		{
//			if (object->GetName() == m_objects[i]->GetName()) {
//				std::string s = object->GetName() + " (" + std::to_string(m_objects.size()) + ")";
//				object->SetName(s);
//				break;
//			}
//		}
//		m_objects.emplace_back(object);
//	}
//}

//void Scene::UpdateAllObjectsInScene(const float dt)
//{
//	m_mainCamera.UpdateWorldTransform();
//
//	for (UINT i = 0; i < m_objects.size(); i++)
//	{
//		if (m_objects[i]->GetState() == Object::STATE::ACTIVE) {
//			m_objects[i]->UpdateWorldTransform();
//			m_objects[i]->Update(dt);			
//		}
//	}
//}

//Camera& Scene::GetMainCamera()
//{
//	return m_sceneRegistry.get<Camera>(m_mainCamera);
//}

void Scene::UpdateTransforms()
{
	auto view = m_sceneRegistry.view<Transform, GameEntity>();
	for (auto entity : view)
	{
		Transform& transform = view.get<Transform>(entity);	
		const Vector3& pos   = transform.m_position;
		const Vector3& scale = transform.m_scale;

		transform.m_last = transform.m_world;

		Mat4f mat;
		DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&transform.m_rotation);
		mat  = DirectX::XMMatrixScaling(transform.m_scale.x, transform.m_scale.y, transform.m_scale.z);
		mat *= DirectX::XMMatrixRotationQuaternion(quatv);
		mat *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		transform.m_local = mat;
		transform.m_world = !transform.Parent() ? transform.m_world = transform.m_local : transform.m_local * transform.Parent()->m_world;
	}
}



void Scene::ResetAllObjectsInScene()
{

	//for (UINT i = 0; i < m_objects.size(); i++)
	//{
	//	if(m_objects[i]->GetState() != Object::STATE::DESTROY)
	//		m_objects[i]->Reset();
	//}
}

void Scene::CleanUpDestroyedObj()
{
	auto view = m_sceneRegistry.view<GameEntity>();
	for (auto entity : view) {
		GameEntity& gEntt = view.get<GameEntity>(entity);
		if (gEntt.m_state == GameEntity::STATE::DESTROY)
			m_sceneRegistry.destroy(entity);
	}
}

//Object* Scene::GetSpecificObject(std::string objectName)
//{
//	UINT id = 0;
//	for ( UINT i = 0; i < m_objects.size(); i++)
//	{
//		if (m_objects[i]->GetName() == objectName) {
//			return m_objects[i];
//		}
//	}
//	return nullptr;
//}

//void Scene::SetLastTransform()
//{
//	m_mainCamera.SetLastTransform();
//
//	for(auto* obj : m_objects )
//	{
//		obj->SetLastTransform();
//	}
//}
