#include "SpotLightHeader.hlsli"
#include "LightFunctionsHeader.hlsli"

LightOutput main(VertexToPixel input) {
	
    float3 worldPosition = worldPositionTexture.Sample(defaultSample, input.m_uv);
	
    float3 lightDir = normalize(-input.m_direction);
    float3 toLight  = input.m_position.xyz - worldPosition; 
    
    float theta = dot(lightDir, normalize(-toLight));
    float cutOff = cos(radians(input.m_cutOff));
	
    LightOutput output;
    output.m_brightColor = 0.0f.rrrr;
    output.m_sceneColor  = 0.0f.rrrr;
    
    if (theta > cutOff)
    {
        float4 albedo = albedoTexture.Sample(defaultSample, input.m_uv).rgba;
        float4 normal = normalTexture.Sample(defaultSample, input.m_uv).rgba;
        float4 material = materialTexture.Sample(defaultSample, input.m_uv);
        float3 vertexNormal = vertexNormalTexture.Sample(defaultSample, input.m_uv).xyz;
    
        float metallic = material.r;
        float roughness = material.g;
        float height = material.b;
        float ao = material.w;
    
        float3 specularcolor = lerp((float3) 0.04, albedo.rgb, metallic);
        float3 diffusecolor = lerp((float3) 0.00, albedo.rgb, 1 - metallic);

        albedo.rgb   = GammaToLinear(albedo.rgb);
        float3 toEye = normalize(CameraPosition.xyz - worldPosition);
        
        float3 lightColor = LinearToGamma(input.m_color.rgb);
        
        float outerCutOff = cos(radians(input.m_outerCutOff));
        float epilson     = outerCutOff - cutOff;
        
        float intensity = clamp((theta - outerCutOff) / epilson, 0.0f, 1.0f);
        float lightDistance = length(toLight);
        
        toLight = normalize(toLight);
        
        float r = reflect(toEye, -normal.xyz);
        
        float3 radiance = EvaluatePointLight(diffusecolor * intensity, specularcolor * intensity, normal.xyz, roughness, lightColor * input.m_color.a, input.m_range, toLight, lightDistance, toEye);

        output.m_sceneColor.rgb = radiance;
        output.m_sceneColor.a   = 1.0f;
    }
    
	return output;
}