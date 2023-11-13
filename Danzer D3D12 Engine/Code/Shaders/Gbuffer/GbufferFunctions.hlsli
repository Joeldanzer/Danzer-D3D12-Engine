#include "GbufferHeader.hlsli"

float GetAmbientOcclusion(VertexToPixel input)
{
    return aoTexrure.Sample(defaultSampler, input.m_uv).r;
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
    return metallicTexture.Sample(defaultSampler, input.m_uv).g;
}

float GetHeight(VertexToPixel input)
{
    return metallicTexture.Sample(defaultSampler, input.m_uv).b;
}

float4 GetNormal(VertexToPixel input)
{
    float4 normal = normalTexture.Sample(defaultSampler, input.m_uv);
    float3 normalAO = normal.xyz;
    normalAO = normalAO * 2.f - 1.f; 
    normalAO = normalize(normalAO);
    //float3 normalAO = (normal.wy, 0.f);
    //normalAO.z = sqrt(1 - saturate(normalAO.x * normalAO.x + normalAO.y * normalAO.y));
    //normalAO = normalize(normalAO);
    
    float3x3 tangentspacematrix = float3x3(normalize(input.m_tangent.xyz), normalize(input.m_biNormal.xyz), normalize(input.m_normal.xyz));
    tangentspacematrix = transpose(tangentspacematrix);
    normalAO = mul(tangentspacematrix, normalAO.xyz);
    normalAO = normalize(normalAO);
    
    float4 output;
    output.xyz = normalAO.xyz;
    output.a = 1.0f;
    return output;
}