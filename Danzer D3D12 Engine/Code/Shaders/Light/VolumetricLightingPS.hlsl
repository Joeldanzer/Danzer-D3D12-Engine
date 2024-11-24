#include "../Fullscreen/FullscreenHeader.hlsli"
#include "VolumetricLightHeader.hlsli"

#define PI 3.14159265359f

static float ditherPattern[4][4] =
{
    { 0.0f, 0.5f, 0.125f, 0.625f },
    { 0.75f, 0.22f, 0.875f, 0.375f },
    { 0.1875f, 0.6875f, 0.0625f, 0.5625 },
    { 0.9375f, 0.4375f, 0.8125f, 0.3125 }
};

float ComputeScattering(float lightDotView)
{
    float result = 1.0f - G_Scattering * G_Scattering;
    result /= (4.0f * PI * pow(1.0f + G_Scattering * G_Scattering - (2.0f * G_Scattering) * lightDotView, ScatteringStrength));
    return result;
}

float4 main(VertexToPixel input) : SV_TARGET
{    
    uint2 windowSize   = uint2(ScreenWidth, ScreenHeight);
    uint2 fragPosition = windowSize * input.m_uv;
    
    float3 worldPosition  = WorldPositionTexture.Sample(samplerState, input.m_uv); 
    float3 rayOrigin      = CameraPositon.xyz;
    float3 endRayPosition = worldPosition;
    float3 rayVector      = endRayPosition - rayOrigin;
    
    float  rayLength = length(rayVector);
    float3 rayDirection = rayVector / rayLength;
    
    float stepLength = rayLength / NUMBER_OF_STEPS;
    float3 step = rayDirection * stepLength;
    
    float3 currentPosition = rayOrigin;

    float3 accumFog = 0.0f.xxx;
    
    for (int i = 0; i < NUMBER_OF_STEPS; i++)
    {
        float4 shadowCameraSpace = mul(float4(currentPosition, 1.0f), LightMatrix);
        shadowCameraSpace        = mul(shadowCameraSpace, LightProjection);    
        shadowCameraSpace       /= shadowCameraSpace.w;
        
        float2 shadowTexCoord = shadowCameraSpace.xy;
        shadowTexCoord   = 0.5f * shadowTexCoord + 0.5f;
        shadowTexCoord.y = 1.0f - shadowTexCoord.y;
        
        //float4 viewSpace = mul(float4(currentPosition, 1.0f), ViewMatrix);
        //viewSpace = mul(viewSpace, ProjectionMatrix);
        //viewSpace.xyz /= viewSpace.w;
        //viewSpace.xy = 0.5f * viewSpace.xy + 0.5f;
        //viewSpace.y =  1.0f - viewSpace.y;
        
        
        float shadowMapValue = ShadowDepthTexture.Sample(samplerState, shadowTexCoord.xy).r;
        if(shadowMapValue + 0.0001f > shadowCameraSpace.z)
            accumFog += (ComputeScattering(dot(LightDirection.xyz, rayDirection)).xxx * LightColor.rgb) * LightColor.w;
        
        //uint2 fragPosition = windowSize * viewSpace.xy;
        currentPosition += step; // * ditherPattern[fragPosition.x % 4][fragPosition.y % 4];
    }
        
    accumFog /= NUMBER_OF_STEPS;
    
    return float4(accumFog, 1.0f);
}