#include "FullscreenHeader.hlsli"

cbuffer FilterData : register(b0)
{
    float3 Offset;
    uint   Radius;
    uint2  WindowSize;
    
    float2 trashOne;
    float4 trashTwo[14];
}

SamplerState Sampler   : register(s0);
Texture2D SceneTexture : register(t0);

float4 main(VertexToPixel input) : SV_TARGET
{    
    float2 srcSize = float2(1.0f / WindowSize.x, 1.0f / WindowSize.y);
    
    float   n = (Radius + 1) * (Radius + 1);
    float3 m0 = Offset; float3 m1 = Offset; float3 m2 = Offset; float3 m3 = Offset;
    float3 s0 = Offset; float3 s1 = Offset; float3 s2 = Offset; float3 s3 = Offset;
    
    float3 c;
    
    for (int i = -Radius; i <= 0; i++)
    {
        for (int j = -Radius; j <= 0; j++)
        {
            c = SceneTexture.Sample(Sampler, input.m_uv + float2(j, i) * srcSize).rgb;
            m0 += c;
            s0 += c * c;
        }

    }
    
    for (int i = -Radius; i <= 0; i++)
    {
        for (int j = 0; j <= Radius; j++)
        {
            c = SceneTexture.Sample(Sampler, input.m_uv + float2(j, i) * srcSize).rgb;
            m1 += c;
            s1 += c * c;
        }

    }
    
    for (int i = 0; i <= Radius; i++)
    {
        for (int j = 0; j <= Radius; j++)
        {
            c = SceneTexture.Sample(Sampler, input.m_uv + float2(j, i) * srcSize).rgb;
            m2 += c;
            s2 += c * c;
        }

    }
    
    for (int i = 0; i <= Radius; i++)
    {
        for (int j = -Radius; j <= 0; j++)
        {
            c = SceneTexture.Sample(Sampler, input.m_uv + float2(j, i) * srcSize).rgb;
            m3 += c;
            s3 += c * c;
        }

    }
    
    float4 color;
    
    float minSigma2 = 1e+2;
    m0 /= n;
    s0 = abs(s0 / n - m0 * m0);
    
    float sigma2 = s0.r + s0.g + s0.b;
    if (sigma2 < minSigma2)
    {
        minSigma2 = sigma2;
        color = float4(m0, 1.0f);
    }
    
    m1 /= n;
    s1 = abs(s1 / n - m1 * m1);
    
    sigma2 = s1.r + s1.g + s1.b;
    if (sigma2 < minSigma2)
    {
        minSigma2 = sigma2;
        color = float4(m1, 1.0f);
    }
    
    m2 /= n;
    s2 = abs(s2 / n - m2 * m2);
    
    sigma2 = s2.r + s2.g + s2.b;
    if (sigma2 < minSigma2)
    {
        minSigma2 = sigma2;
        color = float4(m2, 1.0f);
    }
    
    m3 /= n;
    s3 = abs(s3 / n - m3 * m3);
    
    sigma2 = s3.r + s3.g + s3.b;
    if (sigma2 < minSigma2)
    {
        minSigma2 = sigma2;
        color = float4(m3, 1.0f);
    }
    
    return SceneTexture.Sample(Sampler, input.m_uv);
}