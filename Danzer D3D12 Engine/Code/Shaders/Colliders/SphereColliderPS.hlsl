#include "SphereColliderHeader.hlsli"

Plane ConstructPlane(float3 pos, float3 normal)
{
    Plane newPlane;
    newPlane.m_normal   = normalize(normal);
    newPlane.m_distance = dot(normal, pos);
    return newPlane;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    Plane xPlane = ConstructPlane(input.m_position.xyz, float3(1.0f, 0.0f,  0.0f));
    Plane yPlane = ConstructPlane(input.m_position.xyz, float3(0.0f, 1.0f,  0.0f));
    Plane zPlane = ConstructPlane(input.m_position.xyz, float3(0.0f, 0.0f, -1.0f));
    
    
    

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}