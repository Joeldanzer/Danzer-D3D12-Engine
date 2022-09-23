#include "GbufferHeader.hlsli"

float GetAmbientOcclusion(VertexToPixel input)
{
    return normalTexture.Sample(defaultSampler, input.m_uv).b;
}

float GetEmissive(VertexToPixel input)
{
    return matrerialTexture.Sample(defaultSampler, input.m_uv).b;
}

float GetMetallic(VertexToPixel input)
{
    return matrerialTexture.Sample(defaultSampler, input.m_uv).r;
}

float GetRoughness(VertexToPixel input)
{
    return matrerialTexture.Sample(defaultSampler, input.m_uv).g;
}

float4 GetNormal(VertexToPixel input)
{
    float4 normal = normalTexture.Sample(defaultSampler, input.m_uv);
    float3 normalAO = float3(normal.wy, 0.f);
   
    normalAO = (normalAO * 2.f) - 1.f;
    normalAO.z = sqrt(1 - saturate(normalAO.x * normalAO.x + normalAO.y * normalAO.y));
    normalAO = normalize(normalAO);
    
    float3x3 tangentspacematrix = float3x3(normalize(input.m_tangent.xyz), normalize(input.m_biNormal.xyz), normalize(input.m_normal.xyz));
    normalAO = mul(normalAO.xyz, tangentspacematrix);
    normalAO = normalize(normalAO);
    
    float4 output;
    output.xyz = normalAO.xyz;
    output.a = 1.0f;
    return output;
}