#include "../Light/LightHeader.hlsli"
#include "../Fullscreen/FullscreenHeader.hlsli"

#include "LightFunctionsHeader.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;
    float PI = PI_MACRO;
    
    float4 albedo       = albedoTexture.Sample(defaultSample, input.m_uv);
    float4 normal       = normalTexture.Sample(defaultSample, input.m_uv); 
    float4 material     = materialTexture.Sample(defaultSample, input.m_uv); 
    
    float metallic  = material.r;
    float emissive  = material.g;
    float roughness = material.b;
  
    float3 F0 = 0.04f;
    //F0 = lerp(F0, albedo.rgb, metallic);
    
    float4 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv);
	
    float3 lightDir = normalize(LightDirection);
    float diff = max(dot(normal.rgb, lightDir), 0.f);
    float3 diffuse = (LightColor.rgb * LightColor.w) * diff * albedo.rgb;
    
    float3 V = normalize(CameraPosition.xyz - input.m_position.xyz);
    float3 reflectDir = reflect(lightDir, V);
    float spec = pow(max(dot(V, reflectDir), 0.f), roughness);
    float3 specular = spec * material.rgb;
    //float3 viewDir = normalize(CameraPosition - )
    
    float3 result = (AmbientColor.rgb * AmbientColor.w) + diffuse * spec;
    color.rgb = result;
    //
    //float3 L = LightDirection;
    //float3 H = normalize(V - lightDir);
    
    //float distance = length(CameraPosition);
    //float attenuation = 1.0f / (distance * distance);
    //float3 radiance = (LightColor.rgb * LightColor.w) * attenuation;
    //
    //float NDF = DistributionGGX(normal.rgb, H, roughness);
    //float   G = GeometrySmith(normal.rgb, V, -lightDir, roughness);
    //float3  F = FresnelSchlick(max(dot(H, V), 0.f), F0);
    //
    //float3 kS = F;
    //float3 kD = 1.0 - kS;
    //kD *= 1.0f - metallic;
    //
    //float3 numerator = NDF * G * F;
    //float  denominator = 4.0f * max(dot(normal.rgb, V), 0.f) * max(dot(normal.rgb, -lightDir), 0.f) + 0.0001f;
    //float3 specular = spec * (numerator / denominator);
    //
    //float NdotL = max(dot(normal.rgb, L), 0.f);
    //float3 Lo = (kD * albedo.rgb / PI + specular) * radiance * NdotL;
	//
    //float3 ambient = (AmbientColor.rgb * AmbientColor.w) * albedo.rgb * normal.a;
    //color.rgb = ambient + diffuse + spec;
    color.a = 1.f;
    
    //color.rgb = color.rgb / (color.rgb + 1.0f);
    //color.rgb = pow(color.rgb, 1.0f / 2.2f);
   
	return color;
}