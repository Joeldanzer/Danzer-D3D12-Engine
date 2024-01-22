#include "EffectShaderHeader.hlsli"
#include "WaterPlaneHeader.hlsli"

VertexToPixel main(InputToVertex input) 
{
	VertexToPixel output;
    
    float3 worldPos    = mul(input.m_position, input.m_transform).xyz;
    float  height      = vertexNoise.SampleLevel(effectSampler, worldPos.xz / NoiseScale + Time * Speed, 0.0f).r;
    
    input.m_position.y += height * HeightScale;
    
    float4 toWorld = mul(input.m_position, input.m_transform); 
    float4 toCamera = mul(toWorld, CameraTransform);
    float4 toProj   = mul(toCamera, CameraProjection);
   
    output.m_position = toProj;
    
    //output.m_normal  = normalize(mul(float4(input.m_normal.xyz, 1.f),  input.m_transform));
    //output.m_tangent = normalize(mul(float4(input.m_tangent.xyz, 1.f), input.m_transform));
    //output.m_biNormal = normalize(mul(float4(input.m_biNormal.xyz, 1.0f), input.m_transform));
    //output.m_normal  = float4(input.m_normal.xyz, 1.f);
    //output.m_tangent = float4(input.m_tangent.xyz, 1.f);
    //output.m_tangent = normalize(output.m_tangent - dot(output.m_tangent, output.m_normal) * output.m_normal);
    output.m_normal        = float4(input.m_normal.xyz,   1.f);
    output.m_tangent       = float4(input.m_tangent.xyz,  1.f);
    output.m_biNormal      = float4(input.m_biNormal.xyz, 0.0f);
    output.m_color         = input.m_color;
    output.m_uv            = input.m_uv;
    output.m_worldPosition = toWorld;
    output.m_noiseWorldPos = worldPos;
    return output;
}