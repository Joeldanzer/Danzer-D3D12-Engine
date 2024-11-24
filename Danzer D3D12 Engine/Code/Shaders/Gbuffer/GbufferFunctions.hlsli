#include "GbufferHeader.hlsli"

float GetAmbientOcclusion(VertexToPixel input)
{
    float4 ao = aoTexture.Sample(defaultSampler, input.m_uv);
    ao.r = ao.w == 0.0f ? 1.0f : ao.r;
    return ao.r;
}

float GetEmissive(VertexToPixel input)
{
    //return matrerialTexture.Sample(defaultSampler, input.m_uv).b;
    return 1.f;
}

float GetMetallic(VertexToPixel input)
{
    return metallicTexture.Sample(defaultSampler, input.m_uv).r;
}

float GetRoughness(VertexToPixel input)
{
    return roughnessTexture.Sample(defaultSampler, input.m_uv).r;
}

float GetHeight(VertexToPixel input)
{
    return heightTexture.Sample(defaultSampler, input.m_uv).r;
}

float4 GetNormal(VertexToPixel input)
{
    float3 normal = normalize(input.m_normal.xyz);
    float3 tangent = normalize(input.m_tangent.xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    
    float3 bitangent = cross(tangent, normal);
    float3 bumpMap   = normalTexture.Sample(defaultSampler, input.m_uv.xy);
    bumpMap = bumpMap * 2.0f - 1.0f;
    
    float3x3 TBM = float3x3(tangent, bitangent, normal);
    float3 newNormal = mul(bumpMap, TBM);
    newNormal = normalize(newNormal);
    
    return float4(newNormal, 1.0f);
    
    //float4 normal = normalTexture.Sample(defaultSampler, input.m_uv.xy);
    //float3 normalAO = float3(normal.xy, 0.0f);
    //normalAO.xy = (normalAO * 2.f) - 1.f;
    //normalAO.z = sqrt(1 - saturate(normalAO.x * normalAO.x + normalAO.y * normalAO.y));
    //
    //float3x3 tangentspacematrix = float3x3(normalize(input.m_tangent.xyz), normalize(input.m_biNormal.xyz), normalize(input.m_normal.xyz));
    //normalAO = mul(normalAO.xyz, tangentspacematrix);
    //normalAO = normalize(normalAO);
    //
    //float4 output;
    //output.xyz = normalAO.xyz;
    //output.a = 1.0f;
    //return output;
}