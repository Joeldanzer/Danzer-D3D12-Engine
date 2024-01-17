#include "EffectShaderHeader.hlsli"
#include "WaterPlaneHeader.hlsli"

float4 GetNormal(VertexToPixel input, float4 normal)
{
    float3x3 tangentspacematrix = float3x3(normalize(input.m_tangent.xyz), normalize(input.m_biNormal.xyz), normalize(input.m_normal.xyz));
    tangentspacematrix = transpose(tangentspacematrix);
    
    float3 normalAO = normalize(normal.xyz);
    //normalAO.xyz = normalAO * 2.f - 1.f;
    normalAO.xyz = normalAO * 0.5f + 0.5f;
    normalAO = mul(tangentspacematrix, normalAO.xyz);
    normalAO = normalize(normalAO);
    
    float4 output;
    output.xyz = normalAO.xyz;
    //output.xyz = normal.xyz;
    output.a = 1.0f;
    return output;
}

struct WaterPlaneOutput {
	float4 m_Albedo        : SV_TARGET0;
	float4 m_Normal        : SV_TARGET1;
	float4 m_Material      : SV_TARGET2;
	float4 m_VertexColor   : SV_TARGET3;
	float4 m_VertexNormal  : SV_TARGET4;
	float4 m_WorldPosition : SV_TARGET5;
};

float Fresnel(float amount, float3 normal, float3 view)
{
    return pow((1.0f - clamp(dot(normalize(normal), normalize(view)), 0.0f, 1.0f)), amount);
}

WaterPlaneOutput main(VertexToPixel input)
{	
	WaterPlaneOutput output;

    float2 dirOne = (Time * float2(1.0f,  0.2f)) ;
    float2 dirTwo = (Time * float2(-0.5f, -1.0f));
    
    float4 normalTwo = GetNormal(input, noiseNormalTwo.Sample(effectSampler, (input.m_noiseWorldPos.xz * 3.0f + dirTwo) * 0.1f));
    float4 normalOne = GetNormal(input, noiseNormalOne.Sample(effectSampler, (input.m_noiseWorldPos.xz * 3.0f + dirOne) * 0.1f));
    
    float3 normalBlend = lerp(normalOne.rgb, normalTwo.rgb, 0.5f);
    output.m_Normal    = float4(normalBlend, 1.0f);
    
    float fresnel = Fresnel(1.0f, input.m_normal.xyz, Eye.xyz);
    
    float3 albedoOne = float3(0.1f, 0.5f, 0.95f);
    float3 albedoTwo = float3(0.2f, 0.6, 1.0f);
    
    output.m_Albedo.rgb = lerp(albedoOne, albedoTwo, fresnel);
    output.m_Albedo.a   = 1.0f;
    output.m_Material   = float4(0.5f, 0.1f, 0.5f, 0.5f);
    
    output.m_VertexNormal  = input.m_normal;
    output.m_VertexColor   = input.m_color;
    output.m_WorldPosition = input.m_worldPosition;
    
	return output;
}
