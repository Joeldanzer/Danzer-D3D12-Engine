#pragma once
#include "Core/MathDefinitions.h"

#include "entt/entt.hpp"

#include <string>
#include <vector>

struct Object;
class RayCollider;
class AABBCollider;
class SphereCollider;
class Collider;
class Scene;

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();
	void UpdateCollisions(Scene& scene);
private:

	void CheckAABBCollision(Scene& scene, AABBCollider& col, Object& colObj, entt::entity colEntity);
	void CheckRayCollision(Scene& scene, RayCollider& col, Object& colObj, entt::entity colEntity);
	void CheckSphereCollision(Scene& scene, SphereCollider& col, Object& colObj, entt::entity colEntity);

	bool AABBvsAABB(AABBCollider& col1, AABBCollider& col2);
	bool AABBvsSphere(AABBCollider& col1, SphereCollider& col2);
	bool AABBvsRay(AABBCollider& col1, RayCollider& col2);

	bool PointInside(AABBCollider& col1, Vect3f point);

	//bool SpherevsRay(SphereCollider& col1, RayCollider& col2);
	bool SpherevsSphere(SphereCollider& col1, SphereCollider& col2);

	bool HasIntersectedWithEachother(Collider& col1, entt::entity);
	bool ObjectIsActive(Object& obj);
};

