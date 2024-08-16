#include "stdafx.h"
#include "CollisionManager.h"
#include "Components/Collision/AABBCollider.h"
#include "Components/Collision/RayCollider.h"
#include "Components/Collision/SphereCollider.h"
#include "Components/GameEntity.h"
#include "Components/Transform.h"

#include "Scene.h"

CollisionManager::CollisionManager(){}
CollisionManager::~CollisionManager(){}

void CollisionManager::UpdateCollisions(Scene& scene)
{

	entt::registry& reg = scene.Registry();

	//* AABBCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, AABBCollider>();
		for (auto entity : aabbView)
		{
			GameEntity& obj = reg.get<GameEntity>(entity);
			if (!ObjectIsActive(obj))
				continue;

			Transform& transform = reg.get<Transform>(entity);
			AABBCollider& collider = reg.get<AABBCollider>(entity);

			collider.m_position = transform.m_position + collider.m_offset;
			collider.UpdateMinAndMax();

			CheckAABBCollision(scene, collider, obj, entity);
		}
	}
	//* AABBCollider view end 


	//* SphereCollider view start 
	{
		auto sphereView = reg.view<Transform, GameEntity, AABBCollider>();
		for (auto entity : sphereView)
		{
			GameEntity& obj = reg.get<GameEntity>(entity);
			if (!ObjectIsActive(obj))
				continue;

			Transform& transform = reg.get<Transform>(entity);
			SphereCollider& collider = reg.get<SphereCollider>(entity);

			collider.m_position = transform.m_position + collider.m_offset;
			CheckSphereCollision(scene, collider, obj, entity);
		}
	}
	//* SphereCollider view end 
}

void CollisionManager::CheckAABBCollision(Scene& scene, AABBCollider& col, GameEntity& colObj, entt::entity colEntity)
{
	entt::registry& reg = scene.Registry();

	//* AABBCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, AABBCollider>();
		for (auto entity : aabbView)
		{
			if (entity != colEntity) {
				GameEntity& obj = reg.get<GameEntity>(entity);
				if (obj.m_state != GameEntity::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
					continue;

				Transform& transform = reg.get<Transform>(entity);
				AABBCollider& collider = reg.get<AABBCollider>(entity);
				
				collider.m_position = transform.m_position + collider.m_offset;
				collider.UpdateMinAndMax();

				if (AABBvsAABB(col, collider)) {
					collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
					col.m_intersectList.emplace_back(colObj.m_tag, entity);
				}
			}
		}
	}
	//* AABBCollider view end 

	//* SphereCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, SphereCollider>();
		for (auto entity : aabbView)
		{
			if (entity != colEntity) {
				GameEntity& obj = reg.get<GameEntity>(entity);
				if (obj.m_state != GameEntity::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
					continue;

				Transform& transform = reg.get<Transform>(entity);
				SphereCollider& collider = reg.get<SphereCollider>(entity);

				collider.m_position = transform.m_position + collider.m_offset;

				if (AABBvsSphere(col, collider)) {
					collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
					col.m_intersectList.emplace_back(colObj.m_tag, entity);
				}
			}
		}
	}
	//* SphereCollider view end 

	//* RayCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, RayCollider>();
		for (auto entity : aabbView)
		{
			if (entity != colEntity) {
				GameEntity& obj = reg.get<GameEntity>(entity);
				if (obj.m_state != GameEntity::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
					continue;

				Transform& transform = reg.get<Transform>(entity);
				RayCollider& collider = reg.get<RayCollider>(entity);

				collider.m_position = transform.m_position + collider.m_offset;

				if (AABBvsRay(col, collider)) {
					collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
					col.m_intersectList.emplace_back(colObj.m_tag, entity);
				}
			}
		}
	}
	//* RayCollider view end 
}

void CollisionManager::CheckRayCollision(Scene& scene, RayCollider& col, GameEntity& colObj, entt::entity colEntity)
{
	entt::registry& reg = scene.Registry();

	//* AABBCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, AABBCollider>();
		for (auto entity : aabbView)
		{
			if (entity != colEntity) {
				GameEntity& obj = reg.get<GameEntity>(entity);
				if (obj.m_state != GameEntity::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
					continue;

				Transform& transform = reg.get<Transform>(entity);
				AABBCollider& collider = reg.get<AABBCollider>(entity);

				collider.m_position = transform.m_position + collider.m_offset;
				collider.UpdateMinAndMax();

				if (AABBvsRay(collider, col)) {
					collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
					col.m_intersectList.emplace_back(colObj.m_tag, entity);
				}
			}
		}
	}
	//* AABBCollider view end 
}

void CollisionManager::CheckSphereCollision(Scene& scene, SphereCollider& col, GameEntity& colObj, entt::entity colEntity)
{
	entt::registry& reg = scene.Registry();

	//* AABBCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, AABBCollider>();
		for (auto entity : aabbView)
		{
			if (entity != colEntity) {
				GameEntity& obj = reg.get<GameEntity>(entity);
				if (obj.m_state != GameEntity::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
					continue;

				Transform& transform = reg.get<Transform>(entity);
				AABBCollider& collider = reg.get<AABBCollider>(entity);

				collider.m_position = transform.m_position + collider.m_offset;
				collider.UpdateMinAndMax();

				if (AABBvsSphere(collider, col)) {
					collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
					col.m_intersectList.emplace_back(colObj.m_tag, entity);
				}
			}
		}
	}
	//* AABBCollider view end 

	//* SphereCollider view start 
	{
		auto aabbView = reg.view<Transform, GameEntity, SphereCollider>();
		for (auto entity : aabbView)
		{
			if (entity != colEntity) {
				GameEntity& obj = reg.get<GameEntity>(entity);
				if (obj.m_state != GameEntity::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
					continue;

				Transform& transform = reg.get<Transform>(entity);
				SphereCollider& collider = reg.get<SphereCollider>(entity);

				collider.m_position = transform.m_position + collider.m_offset;

				if (SpherevsSphere(col, collider)) {
					collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
					col.m_intersectList.emplace_back(colObj.m_tag, entity);
				}
			}
		}
	}
	//* SphereCollider view end 

	//* RayCollider view start 
	//{
	//	auto aabbView = reg.view<Transform, Object, RayCollider>();
	//	for (auto entity : aabbView)
	//	{
	//		if (entity != colEntity) {
	//			Object& obj = reg.get<Object>(entity);
	//			if (obj.m_state != Object::STATE::ACTIVE || HasIntersectedWithEachother(col, entity))
	//				continue;
	//
	//			Transform& transform = reg.get<Transform>(entity);
	//			RayCollider& collider = reg.get<RayCollider>(entity);
	//
	//			collider.m_position = transform.m_position + collider.m_offset;
	//
	//			if (AABBvsRay(col, collider)) {
	//				collider.m_intersectList.emplace_back(obj.m_tag, colEntity);
	//				col.m_intersectList.emplace_back(colObj.m_tag, entity);
	//			}
	//		}
	//	}
	//}
	//* RayCollider view end 
}

bool CollisionManager::AABBvsAABB(AABBCollider& col1, AABBCollider& col2)
{
	bool intersect =   (col1.m_min.x <= col2.m_max.x && col1.m_max.x >= col2.m_min.x) &&
					   (col1.m_min.y <= col2.m_max.y && col1.m_max.y >= col2.m_min.y) &&
					   (col1.m_min.z <= col2.m_max.z && col1.m_max.z >= col2.m_min.z);

	return intersect;
}

bool CollisionManager::AABBvsSphere(AABBCollider& col1, SphereCollider& col2)
{
	float distance = 0.0f;

	// MIN
	if (col2.m_position.x < col1.m_min.x) distance += (col1.m_min.x - col2.m_position.x) * (col1.m_min.x - col2.m_position.x);
	if (col2.m_position.y < col1.m_min.y) distance += (col1.m_min.y - col2.m_position.y) * (col1.m_min.y - col2.m_position.y);
	if (col2.m_position.z < col1.m_min.z) distance += (col1.m_min.z - col2.m_position.z) * (col1.m_min.z - col2.m_position.z);

	// MAX
	if (col2.m_position.x > col1.m_max.x) distance += (col2.m_position.x - col1.m_max.x) * (col2.m_position.x - col1.m_min.x);
	if (col2.m_position.y > col1.m_max.y) distance += (col2.m_position.y - col1.m_max.y) * (col2.m_position.y - col1.m_min.y);
	if (col2.m_position.z > col1.m_max.z) distance += (col2.m_position.z - col1.m_max.z) * (col2.m_position.z - col1.m_min.z);

	if (distance < col2.m_radius * col2.m_radius)
		return true;

	return false;
}

bool CollisionManager::AABBvsRay(AABBCollider& col1, RayCollider& col2)
{
	Vect3f max = col1.m_min;
	Vect3f min = col1.m_max;

	float t1 = (min.x - col2.m_position.x) * col2.m_invDirection.x;
	float t2 = (max.x - col2.m_position.x) * col2.m_invDirection.x;

	float tmin = min(t1, t2);
	float tmax = max(t1, t2);

	t1 = (min.y - col2.m_position.y) * col2.m_invDirection.y;
	t2 = (max.y - col2.m_position.y) * col2.m_invDirection.y;

	tmin = max(tmin, min(min(t1, t2), tmax));
	tmax = min(tmax, max(max(t1, t2), tmax));

	float value = max(tmin, 0.f);

	float t = tmax > value ? value : -1.f;
	if (t >= 0.f) {
		Vect3f point = col2.m_position;
		point += {t / col2.m_invDirection.x, t / col2.m_invDirection.y, t / col2.m_invDirection.z};

		float d = DistanceVect3(col2.m_position, point);

		if (d <= col2.m_distance)
			return PointInside(col1, point);
	}

	return false;

}

bool CollisionManager::PointInside(AABBCollider& col1, Vect3f point)
{
	Vect3f min = col1.m_min;
	Vect3f max = col1.m_max;

	return (point.x <= max.x && point.y <= max.y && point.z <= max.z && 
			point.x >= min.x && point.y >= min.y && point.z >= max.z);
}

bool CollisionManager::SpherevsSphere(SphereCollider& col1, SphereCollider& col2)
{
	float d = DistanceVect3(col1.m_position, col2.m_position);
	return d <= col1.m_radius + col2.m_radius;

}

bool CollisionManager::HasIntersectedWithEachother(Collider& col1, entt::entity entity)
{
	for (unsigned int i = 0; i < col1.m_intersectList.size(); i++) {
		if (col1.m_intersectList[i].second == entity) {
			return true;
		}
	}

	return false;
}

bool CollisionManager::ObjectIsActive(GameEntity& obj)
{
	if (obj.m_static || obj.m_state != GameEntity::STATE::ACTIVE)
		return false;

	return true;
}
