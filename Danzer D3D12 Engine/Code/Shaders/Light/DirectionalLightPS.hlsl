#include "../Fullscreen/FullscreenHeader.hlsli"
#include "LightFunctionsHeader.hlsli"
#include "DirectionalLightHeader.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;
    float PI = PI_MACRO;
    
    float3 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv).xyz;
    if (!(length(worldPosition) > 0))
        discard;
     
    if (albedoTexture.Sample(defaultSample, input.m_uv).a <= 0.0f)
        discard;
    
    float4 albedo        = albedoTexture.Sample(defaultSample, input.m_uv).rgba;
    float4 normal        = normalTexture.Sample(defaultSample, input.m_uv).rgba; 
    float4 material      = materialTexture.Sample(defaultSample, input.m_uv); 
    float3 vertexNormal  = vertexNormalTexture.Sample(defaultSample, input.m_uv).xyz; 
    float  ssao           = ssaoTexture.Sample(defaultSample, input.m_uv).r;
    
    float emissiveData = normal.w;
    float metallic     = material.r;
    float roughness    = material.g;
    float height       = material.b;
    float ao           = material.w;
    
    //albedo.rgb = GammaToLinear(albedo.rgb);
    float3 toEye = normalize(CameraPosition.xyz - worldPosition);
    float3 r = reflect(toEye, normalize(normal.xyz));
    
    float3 specualrcolor = lerp((float3) 0.04, albedo.rgb, metallic);
    float3 diffusecolor  = lerp((float3) 0.00, albedo.rgb, 1 - metallic);
       
    //float3 ambient        = EvaluateAmbience(skyboxTexture, defaultSample, vertexNormal, normal.rgb, toEye, roughness, metallic, albedo.rgb, ao, diffusecolor, specualrcolor, AmbientColor);
    float  shadowData       = ShadowCalculation(float4(worldPosition, 1.0f), normal.xyz, LightDirection.xyz, LightTransform, LightProjection);
    float3 directionalLight = EvaluateDirectionalLight(diffusecolor, specualrcolor, normal.xyz, roughness, LightColor.rgb * LightColor.w, LightDirection.xyz, toEye, metallic) * shadowData;    
    
    //float3 kS = FresnelSchlick(max(dot(normal.xyz, toEye.xyz), 0.0), specualrcolor);
    //float3 kD = 1.0 - kS;
    //kD *= 1.0 - metallic;
    float3 ambientNormal = normal.xyz;
    //ambientNormal.z = 1.0f - ambientNormal.z;
    float3 irradiance = skyboxTexture.SampleLevel(defaultSample, ambientNormal.xyz, GetNumMips(skyboxTexture)).rgb * AmbientColor.rgb;
    irradiance *= AmbientColor.a;
    float3 diffuse = irradiance * (albedo.rgb);
    float3 ambient = (diffusecolor * diffuse) * ssao;
        
    float3 radiance = ambient + directionalLight;

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
            color.rgb = albedo.rgb;
            break;
        case 2:
            color.rgb = normal.xyz;
            break;
        case 3:
            color.rgb = ssao.rrr;
            break;
        case 4:
            color.rgb = float3(roughness, roughness, roughness);
            break;
        case 5:
            color.rgb = float3(metallic, metallic, metallic);
            break;
        case 6:
            color.rgb = float3(height, height, height);
            break;
        case 7:
            color.rgb = float3(ao, ao, ao);
            break;
        case 8:
            color.rgb = float3(1.0f, 1.0f, 1.0f) * shadowData;
            break; 
        default:
            color.rgb = radiance;
            break;
    }
    
    color.a = albedo.a;
   
	return color;
}