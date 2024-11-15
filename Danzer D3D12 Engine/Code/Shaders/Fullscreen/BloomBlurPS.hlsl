#include "FullscreenVS.hlsl"

SamplerState Sampler	: register(s0);
Texture2D    HDRTexture : register(t0);

static bool  Horizontal = true;
static float Weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

float4 main(VertexToPixel input) : SV_TARGET
{
    uint width, height;
    HDRTexture.GetDimensions(width, height);

    float2 texOffset = 1.0 / float2(width, height);
    float3 result = HDRTexture.Sample(Sampler, input.m_uv).rgb * Weight[0];
    
    if (Horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            result += HDRTexture.Sample(Sampler, input.m_uv + float2(texOffset.x * i, 0.0f)).rgb * Weight[i];
            result += HDRTexture.Sample(Sampler, input.m_uv - float2(texOffset.x * i, 0.0f)).rgb * Weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            result += HDRTexture.Sample(Sampler, input.m_uv + float2(texOffset.y * i, 0.0f)).rgb * Weight[i];
            result += HDRTexture.Sample(Sampler, input.m_uv - float2(texOffset.y * i, 0.0f)).rgb * Weight[i];
        }
    }
        
	return float4(result, 1.0f);
}