#include "PointLightHeader.hlsli"
#include "LightFunctionsHeader.hlsli"

float Distance(float3 pos1, float3 pos2)
{
    return sqrt(pow(pos2 - pos1, 2.0f));
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;

    float3 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv).xyz;
    
    float distanceCheck = Distance(worldPosition, input.m_LightPosition.xyz);
    if (distanceCheck > input.m_radius)
        discard;
    
    float4 albedo       = albedoTexture.Sample(defaultSample, input.m_uv).rgba;
    float4 normal       = normalTexture.Sample(defaultSample, input.m_uv).rgba;
    float4 material     = materialTexture.Sample(defaultSample, input.m_uv);
    float3 vertexNormal = vertexNormalTexture.Sample(defaultSample, input.m_uv).xyz;
    
    float metallic  = material.r;
    float roughness = material.g;
    float height    = material.b;
    float ao        = material.w;
    
    float3 specularcolor = lerp((float3) 0.04, albedo.rgb, metallic);
    float3 diffusecolor  = lerp((float3) 0.00, albedo.rgb, 1 - metallic);
    
    albedo.rgb   = GammaToLinear(albedo.rgb);
    float3 toEye = normalize(CameraPosition.xyz - worldPosition);
   
    // calculate per-light radiance
    float3 lightDir   = normalize(input.m_LightPosition.xyz - worldPosition.xyz);
    float  distance   = length(input.m_LightPosition.xyz - worldPosition.xyz);
    float attenuation = (1.0 / (distance * distance)) * input.m_radius;
    float3 radiance   = input.m_color.xyz * attenuation;
    
    float3 pointLight = EvaluateDirectionalLight(diffusecolor, specularcolor, normal.xyz, roughness, radiance * input.m_color.w, lightDir.xyz, toEye.xyz, metallic);
   
    color.rgb = pointLight * ao;
    color.rgb = LinearToGamma(color.rgb);
    
    color.a = 1.0f;
    
	return color;
}