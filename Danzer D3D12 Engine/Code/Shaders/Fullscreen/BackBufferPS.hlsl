#include "FullscreenHeader.hlsli"

SamplerState Sampler       : register(s0);
Texture2D   PresentTexture : register(t0);

float4 main(VertexToPixel input) : SV_TARGET
{
    return float4(PresentTexture.Sample(Sampler, input.m_uv).rgb, 1.0f);
}