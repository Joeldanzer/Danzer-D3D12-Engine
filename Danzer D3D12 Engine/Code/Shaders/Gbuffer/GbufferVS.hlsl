#include "GbufferHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
    VertexToPixel output;
   
    // From object -> world -> camera -> projection
    
    float4 toWorld  = mul(input.m_position, input.m_transform);
    float4 toCamera = mul(toWorld,  CameraTransform );
    float4 toProj   = mul(toCamera, CameraProjection);
   
    output.m_position      = toProj;
    output.m_normal        = normalize(mul(float4(input.m_normal.xyz,   0.f),  input.m_transform));
    output.m_tangent       = normalize(mul(float4(input.m_tangent.xyz,  0.f),  input.m_transform));
    output.m_biNormal.xyz  = normalize(mul(float4(input.m_biNormal.xyz, 0.0f), input.m_transform));
    
    output.m_biNormal.w    = 0;
    output.m_color         = input.m_color;
    output.m_uv            = input.m_uv;
    output.m_worldPosition = toWorld;
    return output;
}