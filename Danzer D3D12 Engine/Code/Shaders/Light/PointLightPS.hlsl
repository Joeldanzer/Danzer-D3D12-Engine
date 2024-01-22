#include "Fullscreen/FullscreenHeader.hlsli"
#include "Light/LightFunctionsHeader.hlsli"
#include "Light/PointLightHeader.hlsli"

float Distance(float3 pos1, float3 pos2)
{
    return sqrt(pow(pos2 - pos1, 2.0f));
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;
    float PI = PI_MACRO;
    
    float3 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv).xyz;
    if (!(length(worldPosition) > 0))
        discard;
     
    float distanceCheck = Distance(worldPosition, LightPosition);
    if (distanceCheck > LightRange)
        discard;
    
    if (albedoTexture.Sample(defaultSample, input.m_uv).a <= 0.0f)
        discard;
    
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
    float3 H          = normalize(toEye + lightDir);
    float distance    = length(LightPosition.xyz - worldPosition.xyz);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance   = LightColor.xyz * attenuation;
    
    float3 pointLight = EvaluateDirectionalLight(diffusecolor, specualrcolor, normal.xyz, roughness, radiance * LightColor.w, lightDir.xyz, toEye.xyz, metallic);
   
    color.rgb = pointLight + albedo.rgb;
    color.rgb = LinearToGamma(color.rgb);
    
    color.a = 1.0f;
    
	return color;
}