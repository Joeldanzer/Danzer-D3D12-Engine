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
    float4 color           = float4(MaterialColor.rgb, 1.f);
    output.m_Albedo		   = albedoTexture.Sample(defaultSampler, input.m_uv) * color;	
    clip(output.m_Albedo.a < 0.1f ? -1 : 1);
    output.m_Normal.rgb    = GetNormal(input).rgb;
    output.m_Normal.w	   = GetEmissive(input) * Emissive;
    //output.m_Material      = float4(GetMetallic(input) * Metallic, GetRoughness(input) * Roughness, GetHeight(input), GetAmbientOcclusion(input));
    output.m_Material      = float4(GetMetallic(input), GetRoughness(input), GetHeight(input), GetAmbientOcclusion(input));
    output.m_VertexNormal  = float4(input.m_normal.xyz, 1.f);
    output.m_VertexColor   = input.m_tangent;
    output.m_WorldPosition = input.m_biNormal;

	return output;
}