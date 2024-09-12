#include "FullscreenHeader.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D    SSAOTexture    : register(t0);

float4 main(VertexToPixel input) : SV_TARGET
{
    uint2 textureDimension;
    uint trash;
    SSAOTexture.GetDimensions(0, textureDimension.x, textureDimension.y, trash);
    
    float2 texelSize = 1 / textureDimension;
    float result = 0.0f;
    for (int x = -2; x < 2; x++)
    {
        for (int y = -2; y < 2; y++)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            result += SSAOTexture.Sample(defaultSampler, input.m_uv + offset).r;
        }
    }

    return result / (4.0f * 4.0f);
}
