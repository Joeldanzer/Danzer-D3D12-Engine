#include "../Fullscreen/FullscreenHeader.hlsli"

cbuffer BlurData : register(b0)
{
    uint4    WindowSize; 
    float4   trashTwo[15];
}

SamplerState samplerState : register(s0);

Texture2D DepthTexture			 : register(t0);
Texture2D VolumetricLightTexture : register(t1);


float4 main(VertexToPixel input) : SV_TARGET
{
    uint2 fragPosition    = float2(WindowSize.xy) * input.m_uv;  
    uint2 dsFragPosition  = WindowSize.zw * input.m_uv;
    
    float upSampledDepth = DepthTexture.Sample(samplerState, input.m_uv).r;

    
    //float2 downScaledWindow  = float2(WindowSize) * DownSample;
    //uint2 downScaledPosition = downScaledWindow * input.m_uv;
       
    float3 color = 0.0f.rrr;
    float totalWeight = 0.0f;
	
    int xOffset = fragPosition.x % 2 == 0 ? -1 : 1;
    int yOffset = fragPosition.y % 2 == 0 ? -1 : 1;
	
    int2 offsets[] =
    {
        int2(0, 0),
        int2(0, yOffset),
        int2(xOffset, 0),
        int2(xOffset, yOffset)
    };
    
    for (int i = 0; i < 4; i++)
    {
        uint2 newFragPosition = fragPosition + offsets[i];
        
        float2 uv = float2(newFragPosition) / float2(WindowSize.xy);
        
        float3 downScaledColor = VolumetricLightTexture.Sample(samplerState, uv).rgb;
        float downScaledDepth = DepthTexture.Sample(samplerState, uv).r;
        
        float currentWeight = 1.0f;
        currentWeight *= max(0.0f, 1.0f - (0.05f) * abs(downScaledDepth - upSampledDepth));
        color += downScaledColor * currentWeight;
        totalWeight += currentWeight;     
    }
	
    float3 volumetricLight;
    const float epsilon = 0.f;
    volumetricLight.xyz = color / (totalWeight + epsilon);
    
    return float4(volumetricLight, 1.0f);
}