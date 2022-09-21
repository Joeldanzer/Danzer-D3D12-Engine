#include "UIstructs.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = uiTexture.Sample(defaultSampler, input.m_uv); 
	return color;
}