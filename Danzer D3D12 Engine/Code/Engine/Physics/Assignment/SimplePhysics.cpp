#include "stdafx.h"
#include "SimplePhysics.h"

#include "Physics/RayCaster.h"
#include "Physics/Assignment/Components/BoxCollider.h"
#include "Physics/Assignment/Components/SphereCollider.h"

#include "Components/GameEntity.h"

#include "Core/input.hpp"

#include "Core/RegistryWrapper.h"

void SimplePhysics::SimulatePhysics(const float dt)
{
	if (Input::GetInstance().IsKeyPressed('P'))
		m_startPhysics = !m_startPhysics;

	if (m_startPhysics == true) {
		ApplyGravity(dt);
		HandleCollision();
		CheckRayCasts();

		ApplyVelocity(dt);
	}
}

void SimplePhysics::ApplyGravity(const float dt)
{
	// this can be made to look much cleaner with additional functions and using Templates instead but am a little lazy atm.
	auto boxView    = REGISTRY->GetRegistry().view<GameEntity, Transform, BoxCollider>();
	auto sphereView = REGISTRY->GetRegistry().view<GameEntity, Transform, SphereCollider>();

	for (const Entity entity : boxView) {
		BoxCollider& box = REGISTRY->Get<BoxCollider>(entity);
		if (box.m_gravity && !box.m_kinematic)
			box.m_velocity -= Vect3f(0.0f, m_gravityMultiplier * box.m_mass, 0.0f) * dt;
		//else
		//	box.m_velocity = Vect3f::Zero;
	}

	for (const Entity entity : sphereView) {
		SphereCollider& sphere = REGISTRY->Get<SphereCollider>(entity);
		if (sphere.m_gravity && !sphere.m_kinematic)
			sphere.m_velocity -= Vect3f(0.0f, m_gravityMultiplier * sphere.m_mass, 0.0f) * dt;
		//else
		//	sphere.m_velocity = Vect3f::Zero;
	}
}

void SimplePhysics::ApplyVelocity(const float dt)
{
	// Same thing from ApplyGravity can be applied here
	auto boxView    = REGISTRY->GetRegistry().view<GameEntity, Transform, BoxCollider>();
	auto sphereView = REGISTRY->GetRegistry().view<GameEntity, Transform, SphereCollider>();

	for (const Entity entity : boxView) {
		BoxCollider& box     = REGISTRY->Get<BoxCollider>(entity);
		Transform& transform = REGISTRY->Get<Transform>(entity);

		if (!box.m_kinematic) 
			transform.m_position += box.m_velocity * dt;
	}

	for (const Entity entity : sphereView) {
		SphereCollider& sphere = REGISTRY->Get<SphereCollider>(entity);
		Transform& transform   = REGISTRY->Get<Transform>(entity);

		if (!sphere.m_kinematic)
			transform.m_position += sphere.m_velocity * dt;
	}
}

void SimplePhysics::HandleCollision()
{
	auto boxView = REGISTRY->GetRegistry().view<GameEntity, Transform, BoxCollider>();
	auto sphereView = REGISTRY->GetRegistry().view<GameEntity, Transform, SphereCollider>();

	// Box Collision Checking
	{
		for (const Entity boxEntity : boxView)
		{
			for (const Entity other : boxView)
			{
				if (boxEntity == other)
					continue;

				if (BoxToBoxIntersect(boxEntity, other)) {
					BoxCollider& box = REGISTRY->Get<BoxCollider>(boxEntity);
					// DEMONSTRATION FOR COLLIDING 
					box.m_velocity = { 0.0f, 10.0f, 0.0f };
					if (box.m_OnContactAdded) {
						box.m_OnContactAdded(other);
					}
				}
			}

			for (const Entity other : sphereView)
			{
				if (boxEntity == other)
					continue;

				if (BoxToSphereIntersect(boxEntity, other)) {
					BoxCollider& box = REGISTRY->Get<BoxCollider>(boxEntity);
					box.m_velocity = { 0.0f, 10.0f, 0.0f };

					if (box.m_OnContactAdded)
						box.m_OnContactAdded(other);
				}
			}
		}
	}

	// Sphere Collision Checking
	{
		for (const Entity sphereEntity : sphereView)
		{
			for (const Entity other : sphereView)
			{
				if (sphereEntity == other)
					continue;

				if (SphereToShereIntersect(sphereEntity, other)) {
					SphereCollider& sphere = REGISTRY->Get<SphereCollider>(sphereEntity);
					sphere.m_velocity = { 0.0f, 10.0f, 0.0f };

					if (sphere.m_OnContactAdded)
						sphere.m_OnContactAdded(other);
				}
			}

			for (const Entity other : boxView)
			{
				if (sphereEntity == other)
					continue;

				if (BoxToSphereIntersect(other, sphereEntity)) {
					SphereCollider& sphere = REGISTRY->Get<SphereCollider>(sphereEntity);
					sphere.m_velocity = { 0.0f, 10.0f, 0.0f };
					if (sphere.m_OnContactAdded)
						sphere.m_OnContactAdded(other);
				}
			}
		}
	}
}

void SimplePhysics::CheckRayCasts()
{
	std::vector<ARay>& rays = RayCaster::GetInstance().m_rays;
	
	auto boxView    = REGISTRY->GetRegistry().view<GameEntity, Transform, BoxCollider>();
	auto sphereView = REGISTRY->GetRegistry().view<GameEntity, Transform, SphereCollider>();
	
	for (uint32_t i = 0; i < rays.size(); i++)
	{
		for (const Entity entity : boxView)
			RayToBoxIntersect(rays[i], entity);
		
		for (const Entity entity : sphereView)
			RayToSphereIntersect(rays[i], entity);		
	}

	rays.clear();
}


void SimplePhysics::RayToBoxIntersect(const ARay& ray, const Entity box)
{
	BoxCollider& collider = REGISTRY->Get<BoxCollider>(box);
	Transform& transform  = REGISTRY->Get<Transform>(box);

	Mat4f rotation;
	transform.World().Transpose(rotation);
	rotation.Translation(Vector3::Zero);

	Vect3f localOrigin = Mul(rotation, ray.m_origin - transform.m_position);
	Vect3f localDir    = Mul(rotation, ray.m_direction);

	BoxCollider localBox = BoxCollider(collider.m_extents);

	ARay localRay(localOrigin, localDir);

	if (RayEntryForBox(localRay, localBox)) {
		collider.m_velocity += 5.0f * ray.m_direction;
		if (ray.m_RayHit)
			ray.m_RayHit(box, 1.0f);
	}
}

const bool SimplePhysics::RayEntryForBox(const ARay& ray, const BoxCollider& box)
{
	Vect3f min = Vect3f::Zero - box.m_extents;
	Vect3f max = Vect3f::Zero + box.m_extents;

	Vect3f invDir = { 1.0f / ray.m_direction.x, 1.0f / ray.m_direction.y, 1.0f / ray.m_direction.z };

	float t1 = (min.x - ray.m_origin.x) * invDir.x;
	float t2 = (max.x - ray.m_origin.x) * invDir.x;
	float t3 = (min.y - ray.m_origin.y) * invDir.y;
	float t4 = (max.y - ray.m_origin.y) * invDir.y;
	float t5 = (min.z - ray.m_origin.z) * invDir.z;
	float t6 = (max.z - ray.m_origin.z) * invDir.z;

	float tMin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tMax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	return tMax >= max(0.0f, tMin);
}

void SimplePhysics::RayToSphereIntersect(const ARay& ray, const Entity sphere)
{
	SphereCollider& collider  = REGISTRY->Get<SphereCollider>(sphere);
	const Transform&      transform = REGISTRY->Get<Transform>(sphere);

	const Vect3f diff = (transform.m_position + collider.m_center) - ray.m_origin;

	const float t0 = diff.Dot(ray.m_direction);
	const float dSquared = diff.Dot(diff) - t0 * t0;

	const float rSquared = collider.m_radius * collider.m_radius;
	if (dSquared > rSquared)
		return;

	const float t1 = sqrtf(rSquared - dSquared);

	const float Epsilon = 0.00001f;
	float outDistance = (t0 > t1 + Epsilon) ? t0 - t1 : t0 + t1;

	if (outDistance > Epsilon) {
		collider.m_velocity += ray.m_direction * 5.0f;
		if (ray.m_RayHit)
			ray.m_RayHit(sphere, outDistance);
	}
}

const bool SimplePhysics::BoxToSphereIntersect(const Entity boxEntity, const Entity sphereEntity)
{
	SphereCollider& sphere          = REGISTRY->Get<SphereCollider>(sphereEntity);
	Transform&      sphereTransform = REGISTRY->Get<Transform>(sphereEntity);

	BoxCollider&	box				= REGISTRY->Get<BoxCollider>(boxEntity);
	Transform&		boxTransform	= REGISTRY->Get<Transform>(boxEntity);

	Mat4f inverse = Mat4f::Identity;
	boxTransform.World().Invert(inverse);

	Vect3f localSphereCenter = Mul(inverse, sphereTransform.m_position);
	Vect3f closestPoint		 = DirectX::XMVectorClamp(localSphereCenter, -box.m_extents, box.m_extents);

	float dist2 = Vect3f(localSphereCenter - closestPoint).LengthSquared();

	return dist2 < (sphere.m_radius * sphere.m_radius);
}

const bool SimplePhysics::BoxToBoxIntersect(const Entity box1Entity, const Entity box2Entity)
{
	BoxCollider& box1		   = REGISTRY->Get<BoxCollider>(box1Entity);
	Transform&	 box1Transform = REGISTRY->Get<Transform>(box1Entity);

	BoxCollider& box2	       = REGISTRY->Get<BoxCollider>(box2Entity);
	Transform&   box2Transform = REGISTRY->Get<Transform>(box2Entity);

	Mat4f rotation1 = box1Transform.World().CreateTranslation(Vect3f::Zero);
	Mat4f rotation2 = box2Transform.World().CreateTranslation(Vect3f::Zero);

	Vect3f translation = box2Transform.m_position - box1Transform.m_position;
	translation        = Mul(rotation1.Transpose(), translation);

	Mat4f rotation = rotation1.Transpose() * rotation2;
	Mat4f absRotation = Matrix4Abs(rotation);

	//Vect3f halfSize1 = box1.m_extents * 0.5f;
	//Vect3f halfSize2 = box2.m_extents * 0.5f;
	
	if (!SeparatingAxisTheoremForBox(translation, absRotation, box2.m_extents, box1.m_extents))
		return false;

	return true;
}

const bool SimplePhysics::SphereToShereIntersect(const Entity sphere1, const Entity sphere2)
{
	const SphereCollider& sphereColl1      = REGISTRY->Get<SphereCollider>(sphere1);
	const Transform&      sphereTransform1 = REGISTRY->Get<Transform>(sphere1);

	const SphereCollider& sphereColl2      = REGISTRY->Get<SphereCollider>(sphere2);
	const Transform&      sphereTransform2 = REGISTRY->Get<Transform>(sphere2);

	float distance = Vect3f::Distance(sphereTransform1.m_position, sphereTransform2.m_position);

	return distance <= (sphereColl1.m_radius + sphereColl2.m_radius);
}

bool SimplePhysics::SeparatingAxisTheoremForBox(const Vect3f& translation, const Mat4f& absMatrix, const Vect3f& extent1, const Vect3f& extent2)
{
	const float ra[3]			  = { extent1.x, extent1.y, extent1.z };
	const float newTranslation[3] = {translation.x, translation.y, translation.z};

	for (uint32_t i = 0; i < 3; i++)
	{
		const Vect3f absRot3 = { absMatrix.m[i][0], absMatrix.m[i][1], absMatrix.m[i][2] };
		const float  rb = absRot3.Dot(extent2);
		if (std::fabs(newTranslation[i]) > (ra[i] + rb)) return false;
	}

	return true;
}
