#include "../Light/LightHeader.hlsli"
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
    float3 normal        = normalTexture.Sample(defaultSample, input.m_uv).rgb; 
    float3 material      = materialTexture.Sample(defaultSample, input.m_uv).rgb; 
    float3 vertexNormal  = vertexNormalTexture.Sample(defaultSample, input.m_uv).xyz; 
    
    float metallic      = material.r;
    float emissiveData  = material.b;
    float roughness     = material.g;
    float ao = normalTexture.Sample(defaultSample, input.m_uv).a;
    
    float3 toEye = normalize(CameraPosition.xyz - worldPosition);
  
    //float shadowData = PixelShader_Shadow(float4(worldposition, 1.0f));
    
    //float3 r = reflect(toEye, normalize(normal));
    
    float3 specualrcolor = lerp((float3) 0.04, albedo, metallic);
    float3 diffusecolor  = lerp((float3) 0.00, albedo, 1 - metallic);
   
     //diffusecolor *= ao;
    
    float3 ambience = EvaluateAmbience(skyboxTexture, defaultSample, vertexNormal, normal, toEye, roughness, metallic, albedo, ao, albedo, specualrcolor, AmbientColor);
    float3 directionalLight = EvaluateDirectionalLight(diffusecolor, specualrcolor, normal, roughness, LightColor.rgb * LightColor.w, LightDirection.xyz, toEye.xyz);
    //if (shadowData > 0.0f)
    //{
    //}
    //else
    //{
    //    directionalLight = shadowColor.rgb * shadowColor.w;
    //}
    //float3 directionallight = EvaluateDirectionalLight(diffusecolor, specualrcolor, normal, perceptualroughness, myLightColor.rgb * myLightColor.w, myLightDirection.xyz, toEye.xyz);

    float3 emissive = albedo * emissiveData;
    float3 radiance = ambience + directionalLight + emissive;
    
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
            color.rgb = albedo;
            break;
        case 2:
            color.rgb = 0.5f + 0.5f * normal;
            break;
        case 3:
            color.rgb = material;
            break;
        case 4:
            color.rgb = vertexNormal;
            break;
        case 5:
            color.rgb = worldPosition;
        break;
        
        default:
            color.rgb = radiance;
            break;
    }
    
    color.a = 1.f;
   
	return color;
}