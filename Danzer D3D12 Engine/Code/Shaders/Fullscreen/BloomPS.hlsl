#include "FullscreenVS.hlsl"

SamplerState Sampler        : register(s0);
Texture2D DirLightTexture   : register(t0);
Texture2D PointLightTexture : register(t1);
Texture2D SpotLightTexture  : register(t2);
Texture2D BloomBlurTexture  : register(t3);

static float Exposure = 1.0f;

float4 main(VertexToPixel input) : SV_TARGET
{
    const float gamma = 2.2f;
    
    float3 hdrColor = DirLightTexture.Sample(Sampler, input.m_uv).rgb   + 
                      PointLightTexture.Sample(Sampler, input.m_uv).rgb + 
                      SpotLightTexture.Sample(Sampler, input.m_uv).rgb;
    
    float3 bloomColor = BloomBlurTexture.Sample(Sampler, input.m_uv).rgb;
    hdrColor += bloomColor;
    
    float3 result = float(1.0f).rrr - exp(-hdrColor * Exposure);
    //result = pow(result, 1.0f / gamma);
	
    return float4(result, 1.0f);
}