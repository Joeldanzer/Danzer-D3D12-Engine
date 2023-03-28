#include "GbufferHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
    VertexToPixel output;
   
    // From object -> world -> camera -> projectioc
    float4 toWorld =  mul(input.m_transform, input.m_position);
    float4 toCamera = mul(CameraTransform, toWorld);
    float4 toProj =   mul(CameraProjection, toCamera);
    
    output.m_position      = toProj;
    output.m_normal        = mul(input.m_transform, float4(input.m_normal.xyz,   0.f));
    output.m_tangent       = mul(input.m_transform, float4(input.m_tangent.xyz,  0.f));
    output.m_biNormal      = mul(input.m_transform, float4(input.m_biNormal.xyz, 0.f));
    output.m_color         = input.m_color;
    output.m_uv            = input.m_uv;
    output.m_worldPosition = toWorld;
    return output;
}