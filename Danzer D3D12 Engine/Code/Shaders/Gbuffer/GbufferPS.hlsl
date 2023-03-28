#include "GbufferFunctions.hlsli"

struct GBufferOutput {
	float4 m_Albedo        : SV_TARGET0;
	float4 m_Normal        : SV_TARGET1;
	float4 m_Material      : SV_TARGET2;
	float4 m_VertexColor   : SV_TARGET3;
	float4 m_VertexNormal  : SV_TARGET4;
	float4 m_WorldPosition : SV_TARGET5;
};

GBufferOutput main(VertexToPixel input)
{
	GBufferOutput output;
    output.m_Albedo		   = albedoTexture.Sample(defaultSampler, input.m_uv) * MaterialColor;	
    clip(output.m_Albedo.a < 0.1f ? -1 : 1);
    output.m_Normal.rgb    = GetNormal(input).rgb;
    output.m_Normal.w	   = GetAmbientOcclusion(input);
    
    if(HasMaterialTexture == 1)
        output.m_Material = float4(GetMetallic(input) * Metallic, GetRoughness(input) * Roughness, GetEmissive(input) * Emissive, 1.f);
    else
        output.m_Material = float4(Metallic, Roughness, Emissive, 1.f);
    
    output.m_VertexNormal  = float4(abs(input.m_normal.xyz), 1.f);
    output.m_VertexColor   = input.m_color;
    output.m_WorldPosition = input.m_worldPosition;

	return output;
}