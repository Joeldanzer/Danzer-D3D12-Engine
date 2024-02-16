#include "../Fullscreen/FullscreenHeader.hlsli"
#include "LightFunctionsHeader.hlsli"
#include "PointLightHeader.hlsli"

float Distance(float3 pos1, float3 pos2)
{
    return sqrt(pow(pos2 - pos1, 2.0f));
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;

    float3 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv).xyz;
    float distanceCheck = Distance(worldPosition, LightPosition);
    //if (distanceCheck > LightRange)
    //    discard;
    //
    //if (albedoTexture.Sample(defaultSample, input.m_uv).a <= 0.0f)
    //    discard;
    
    float4 albedo = albedoTexture.Sample(defaultSample, input.m_uv).rgba;
    float4 normal = normalTexture.Sample(defaultSample, input.m_uv).rgba;
    float4 material = materialTexture.Sample(defaultSample, input.m_uv);
    float3 vertexNormal = vertexNormalTexture.Sample(defaultSample, input.m_uv).xyz;
    
    float metallic  = material.r;
    float roughness = material.g;
    float height    = material.b;
    float ao        = material.w;
    
    float3 specualrcolor = lerp((float3) 0.04, albedo.rgb, metallic);
    float3 diffusecolor  = lerp((float3) 0.00, albedo.rgb, 1 - metallic);
    
    albedo.rgb = GammaToLinear(albedo.rgb);
    float3 toEye = normalize(CameraPosition.xyz - worldPosition);
   
    // calculate per-light radiance
    float3 lightDir   = normalize(LightPosition.xyz - worldPosition.xyz);
    float  distance    = length(LightPosition.xyz - worldPosition.xyz);
    float attenuation = (1.0 / (distance * distance)) * LightRange;
    float3 radiance   = LightColor.xyz * attenuation;
    
    float3 pointLight = EvaluateDirectionalLight(diffusecolor, specualrcolor, normal.xyz, roughness, radiance * LightColor.w, lightDir.xyz, toEye.xyz, metallic);
   
    color.rgb = pointLight;
    color.rgb = LinearToGamma(color.rgb);
    
    color.a = 1.0f;
    
	return color;
}