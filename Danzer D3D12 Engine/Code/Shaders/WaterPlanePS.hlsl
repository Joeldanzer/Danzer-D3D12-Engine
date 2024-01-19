#include "EffectShaderHeader.hlsli"
#include "WaterPlaneHeader.hlsli"

#define WIDTH  1920.0f;
#define HEIGHT 1080.0f;

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

float EdgeDetection(float depth)
{
    depth = 2.0f * depth - 1.0f;
    return Near * Far / (Far + depth * (Near - Far));
}

struct WaterPlaneOutput {
	float4 m_Albedo        : SV_TARGET0;
	float4 m_Normal        : SV_TARGET1;
	float4 m_Material      : SV_TARGET2;
	float4 m_VertexColor   : SV_TARGET3;
	float4 m_VertexNormal  : SV_TARGET4;
	float4 m_WorldPosition : SV_TARGET5;
    float4 m_Depth         : SV_TARGET6;
};

float Fresnel(float amount, float3 normal, float3 view)
{
    return pow((1.0f - clamp(dot(normalize(normal), normalize(view)), 0.0f, 1.0f)), amount);
}

WaterPlaneOutput main(VertexToPixel input)
{	
	WaterPlaneOutput output;
 
    float3 svPos = input.m_position.xyz;
    svPos.xy /= float2(ScreenWidth, ScreenHeight);

    float zDepth = EdgeDetection(depthTexture.Sample(effectSampler, svPos.xy).x);
    float zPos   = EdgeDetection(svPos.z);
    float zDif   = zDepth - zPos;
    
    float2 dirOne = (Time * WaterDirectionOne) ;
    float2 dirTwo = (Time * WaterDirectionTwo);
    
    float4 normalOne = GetNormal(input, noiseNormalOne.Sample(effectSampler, (input.m_noiseWorldPos.xz * TextureScale + dirOne) * Speed));
    float4 normalTwo = GetNormal(input, noiseNormalTwo.Sample(effectSampler, (input.m_noiseWorldPos.xz * TextureScale + dirTwo) * Speed));
    
    float3 normalBlend = lerp(normalOne.rgb, normalTwo.rgb, 0.5f);
    output.m_Normal    = float4(normalBlend, 1.0f);
    
    float fresnel = Fresnel(5.0f, input.m_normal.xyz, Eye.xyz);
    
    float3 albedoOne = WaterColorOne;
    float3 albedoTwo = WaterColorTwo;
    
    float3 surfaceColor = lerp(albedoOne, albedoTwo, fresnel);
     
    output.m_Albedo.rgb = lerp(EdgeColor, surfaceColor, step(EdgeScale, zDif));
    output.m_Albedo.a   = 1.0f;
    output.m_Material   = float4(Metallic, Roughness, 0.5f, 0.5f);
    
    output.m_Normal.a   = 1.0f;
    output.m_Material.a = 1.0f;
    
    output.m_VertexNormal  = input.m_normal;
    output.m_VertexColor   = input.m_color;
    output.m_WorldPosition = input.m_worldPosition;
    output.m_Depth         = float4(svPos.z, svPos.z, svPos.z, 1.0f);
    
	return output;
}
