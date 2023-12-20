#include "../Fullscreen/FullscreenHeader.hlsli"
#include "LightFunctionsHeader.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;
    float PI = PI_MACRO;
    
    float3 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv).xyz;
    if (!(length(worldPosition) > 0))
        discard;
    
    float3 albedo        = albedoTexture.Sample(defaultSample, input.m_uv).rgb;
    float4 normal        = normalTexture.Sample(defaultSample, input.m_uv).rgba; 
    float4 material      = materialTexture.Sample(defaultSample, input.m_uv); 
    float3 vertexNormal  = vertexNormalTexture.Sample(defaultSample, input.m_uv).xyz; 
    
    float emissiveData = normal.w;
    float metallic     = material.r;
    float roughness    = material.g;
    float height       = material.b;
    float ao           = material.w;
    
    float3 toEye = normalize(CameraPosition.xyz - worldPosition);
  
    float shadowData = ShadowCalculation(float4(worldPosition, 1.0f), normal.xyz);
    
    //float3 r = reflect(toEye, normalize(normal));
    
    float3 specualrcolor = lerp((float3) 0.04, albedo, metallic);
    float3 diffusecolor  = lerp((float3) 0.00, albedo, 1 - metallic);
       
    float3 ambience         = EvaluateAmbience(skyboxTexture, defaultSample, normal.rgb, vertexNormal, toEye, roughness, metallic, albedo, ao, diffusecolor, specualrcolor, AmbientColor);
    float3 directionalLight = EvaluateDirectionalLight(diffusecolor, specualrcolor, normal.rgb, roughness, LightColor.rgb * LightColor.w, LightDirection.xyz, toEye.xyz) * shadowData;    
    float3 emissive = albedo * emissiveData;
    float3 radiance = ambience + directionalLight;
    
    // Fog that i want to get in!
    //float4 oldWorldPos = worldPositionTexture.Sample(defaultSample, input.m_uv).xyzw - CameraPosition.xyzw;
    //float fogDistance = length(oldWorldPos.xyz) / fogDistanceMultiplier;
    //
    //float fogAmount = 1. - exp2(-fogDensity * fogDensity * fogDistance * fogDistance * LOG2);
    //fogAmount = clamp(fogAmount, 0.f, 1.f);
    //
    //float3 newFogColor = enviromentTexture.Sample(defaultSampler, oldWorldPos.xyz).rgb;
    //newFogColor = GammaToLinear(newFogColor);
    //
    //if (fogAmount < fogClip)
    //{
    //    newFogColor = fogColor.rgb;
    //}
    //else
    //{
    //    newFogColor = lerp(fogColor.rgb, newFogColor, fogAmount);
    //}

    //radiance = lerp(radiance.rgb, newFogColor.rgb, fogAmount.rrr);
   
    switch (CameraPosition.w)
    {
        case 0:
            color.rgb = radiance;    
            break;
        case 1:
            color.rgb = normal.rgb * 0.5 + 0.5f;
            break;
        case 2:
            color.rgb = vertexNormal.rgb;
            break;
        case 3:
            color.rgb = directionalLight;
            break;
        case 4:
            color.rgb = float3(metallic, metallic, metallic);
            break;
        case 5:
            color.rgb = float3(roughness, roughness, roughness);
            break;
        case 6:
            color.rgb = float3(height, height, height);
            break;
        case 7:
            color.rgb = float3(ao, ao, ao);
            break;
        case 8:
            color.rgb = worldPosition;
            break; 
        default:
            color.rgb = radiance;
            break;
    }
    
    color.a = 1.f;
   
	return color;
}