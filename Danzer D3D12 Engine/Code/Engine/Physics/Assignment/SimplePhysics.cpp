#include "stdafx.h"
#include "SimplePhysics.h"

#include "Components/Transform.h"

#include "Physics/Assignment/Components/BoxCollider.h"
#include "Physics/Assignment/Components/SphereCollider.h"

#include "Core/RegistryWrapper.h"

bool SimplePhysics::BoxToSphereIntersect(const Entity boxEntity, const Entity sphereEntity)
{
	SphereCollider& sphere     = REGISTRY->Get<SphereCollider>(sphereEntity);
	Transform& sphereTransform = REGISTRY->Get<Transform>(sphereEntity);

	BoxCollider& box		   = REGISTRY->Get<BoxCollider>(boxEntity);
	Transform& boxTransform	   = REGISTRY->Get<Transform>(boxEntity);

	Mat4f inverse = Mat4f::Identity;
	boxTransform.World().Invert(inverse);

	Vect3f localSphereCenter = Mul(inverse, sphereTransform.m_position);
	Vect3f closestPoint = DirectX::XMVectorClamp(localSphereCenter, -box.m_extents, box.m_extents);

	float dist2 = Vect3f(localSphereCenter - closestPoint).Length();

	return dist2 < (sphere.m_radius * sphere.m_radius);
}

bool SimplePhysics::BoxToBoxIntersect(const Entity box1Entity, const Entity box2Entity)
{
	BoxCollider& box1		   = REGISTRY->Get<BoxCollider>(box1Entity);
	Transform&	 box1Transform = REGISTRY->Get<Transform>(box2Entity);

	BoxCollider& box2	       = REGISTRY->Get<BoxCollider>(box1Entity);
	Transform&   box2Transform = REGISTRY->Get<Transform>(box2Entity);

	Mat4f rotation1 = box1Transform.World().CreateTranslation(Vect3f::Zero);
	Mat4f rotation2 = box2Transform.World().CreateTranslation(Vect3f::Zero);

	Vect3f translation = box2Transform.m_position - box1Transform.m_position;
	translation = Mul(rotation1.Transpose(), translation);

	Mat4f rotation = rotation1.Transpose() * rotation2;
	Mat4f absRotation = Matrix4Abs(rotation);

	Vect3f halfSize1 = box1.m_extents * 0.5f;
	Vect3f halfSize2 = box2.m_extents * 0.5f;


	return false;
}
