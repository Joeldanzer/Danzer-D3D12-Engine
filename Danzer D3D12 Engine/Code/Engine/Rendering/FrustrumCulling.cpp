#include "stdafx.h"
#include "FrustrumCulling.h"

#include "Camera.h"
#include "Components/Transform.h"
#include "Rendering/Data/DebugRenderingData.h"

bool FrustrumCulling::MeshIsInFrustrum(const Camera& cam, Transform& transform, const AABB& aabb, DebugRenderingData& debug)
{
    Vect3f globalCenter = Mul(transform.World(), Vect4f(aabb.m_center));

    const Vect3f right   = transform.World().Right()   * aabb.m_extent.x;
    const Vect3f up      = transform.World().Up()      * aabb.m_extent.y;
    const Vect3f forward = transform.World().Forward() * aabb.m_extent.z;

    const float newIi =
        std::abs(Dot(Vect3f::Right, right)) +
        std::abs(Dot(Vect3f::Right, up)) +
        std::abs(Dot(Vect3f::Right, forward));

    const float newIj =
        std::abs(Dot(Vect3f::Up, right)) +
        std::abs(Dot(Vect3f::Up, up)) +
        std::abs(Dot(Vect3f::Up, forward));

    const float newIk =
        std::abs(Dot(Vect3f::Forward, right)) +
        std::abs(Dot(Vect3f::Forward, up)) +
        std::abs(Dot(Vect3f::Forward, forward));
    
    const AABB globalAABB(globalCenter, newIi, newIj, newIk);
    debug.RenderAABB(transform.World(), aabb.m_center, globalAABB.m_extent);

    for (uint32_t i = 0; i < Camera::FACE_COUNT; i++) {
        if (!globalAABB.IsOnForwardPlane(cam.GetFrustrumFace(static_cast<Camera::Faces>(i))))
           return false;
        
    }

    return true;
}
