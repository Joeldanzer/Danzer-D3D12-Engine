#define PI_MACRO 3.14159265359f;

float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0f, 1.f), 5.0f);
}

float4 DistributionGGX(float3 N, float3 H, float roughness)
{
    float PI = PI_MACRO;
    
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.f);
    float NdotH2 = NdotH * NdotH;
 
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    
    
    denom = PI * denom * denom;
    
    return nom / denom; 
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1);
    float k = (r * r) / 8.0f;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.f);
    float NdotL = max(dot(N, L), 0.f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}