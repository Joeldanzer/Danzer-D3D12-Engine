#include "FullscreenVS.hlsl"

SamplerState Sampler       : register(s0);
Texture2D SceneTexture     : register(t0);
Texture2D BloomBlurTexture : register(t1);

static float Exposure = 1.0f;

float4 main(VertexToPixel input) : SV_TARGET
{
    const float gamma = 2.2f;
    
    float3 hdrColor   = SceneTexture.Sample(Sampler, input.m_uv).rgb;
    float3 bloomColor = BloomBlurTexture.Sample(Sampler, input.m_uv).rgb;
    hdrColor += bloomColor;
    
    float3 result = float(1.0f).rrr - exp(-hdrColor * Exposure);
    //result = pow(result, 1.0f / gamma);
	
    return float4(result, 1.0f);
}