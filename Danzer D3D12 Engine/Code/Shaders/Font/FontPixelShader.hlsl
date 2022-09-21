#include "FontStructs.hlsli"

float4 main(GeometryToPixel input) : SV_TARGET
{
    float4 color = fontTexture.Sample(defaultSampler, input.m_UV);
    color *= input.m_color;
	return color;
}