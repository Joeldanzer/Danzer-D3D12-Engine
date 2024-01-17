#include "EffectShaderHeader.hlsli"
#include "WaterPlaneHeader.hlsli"

VertexToPixel main(InputToVertex input) 
{
	VertexToPixel output;
    
    float noiseScale   = 100.f;
    float heightScale  = 0.75f;
    float3 worldPos    = mul(input.m_position, input.m_transform).xyz;
    float  height      = vertexNoise.SampleLevel(effectSampler, worldPos.xz / noiseScale + Time * 0.01f, 0.0f).r;
    
    input.m_position.y += height * heightScale;
    
    float4 toWorld = mul(input.m_position, input.m_transform);
     
    float4 toCamera = mul(toWorld, CameraTransform);
    float4 toProj   = mul(toCamera, CameraProjection);
   
    output.m_position = toProj;
    
    output.m_normal  = normalize(mul(float4(input.m_normal.xyz, 1.f),  input.m_transform));
    output.m_tangent = normalize(mul(float4(input.m_tangent.xyz, 1.f), input.m_transform));
    //output.m_normal  = float4(input.m_normal.xyz, 1.f);
    //output.m_tangent = float4(input.m_tangent.xyz, 1.f);
    //output.m_tangent = normalize(output.m_tangent - dot(output.m_tangent, output.m_normal) * output.m_normal);
    output.m_biNormal.xyz = input.m_biNormal.xyz; //cross(output.m_tangent.xyz, output.m_normal.xyz);
    output.m_biNormal.w = 0;
    output.m_color = input.m_color;
    output.m_uv = input.m_uv;
    output.m_worldPosition = toWorld;
    output.m_noiseWorldPos = worldPos;
    return output;
}