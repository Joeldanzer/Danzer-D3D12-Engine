#include "GbufferHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
    VertexToPixel output;
   
    // From object -> world -> camera -> projectioc
    float4 toWorld =  mul(input.m_transform, input.m_position);
    float4 toCamera = mul(CameraTransform, toWorld);
    float4 toProj =   mul(CameraProjection, toCamera);
    
    output.m_position      = toProj;

    output.m_normal        = mul(input.m_transform, float4(input.m_normal.xyz, 0.f));
    output.m_tangent       = mul(input.m_transform, float4(input.m_tangent.xyz,  0.f));
    output.m_tangent       = normalize(output.m_tangent - dot(output.m_tangent, output.m_normal) * output.m_normal);
    output.m_biNormal.rgb  = cross(output.m_normal.rgb, output.m_tangent.rgb);
    output.m_biNormal.w    = 0;
    //output.m_biNormal      = mul(input.m_transform, float4(input.m_biNormal.xyz, 0.f));
    output.m_color         = input.m_color;
    output.m_uv            = input.m_uv;
    output.m_worldPosition = toWorld;
    return output;
}