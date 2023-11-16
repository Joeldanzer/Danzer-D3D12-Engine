#include "GbufferHeader.hlsli"

float GetAmbientOcclusion(VertexToPixel input)
{
    return aoTexture.Sample(defaultSampler, input.m_uv).r;
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
    float4 normal = normalTexture.Sample(defaultSampler, input.m_uv);
    float3x3 tangentspacematrix = float3x3(normalize(input.m_tangent.xyz), normalize(input.m_biNormal.xyz), normalize(input.m_normal.xyz));
    tangentspacematrix = transpose(tangentspacematrix);
    
    //float3 normalAO = (normal.xy, 0.f);
    //normalAO.z = sqrt(1 - saturate(normalAO.x * normalAO.x + normalAO.y * normalAO.y));
    //normalAO = normalize(normalAO);
    
    float3 normalAO = normal.xyz;
    normalAO.xyz = normalAO * 2.f - 1.f; 
    normalAO = mul(tangentspacematrix, normalAO.xyz);
    normalAO = normalize(normalAO);
    
    float4 output;
    output.xyz = normalAO.xyz;
    output.a = 1.0f;
    return output;
}