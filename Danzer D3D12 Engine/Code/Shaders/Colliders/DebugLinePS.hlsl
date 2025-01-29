#include "DebugLineHeader.hlsli"

float4 main(GeometryToPixel input) : SV_TARGET
{
	return float4(input.m_color.rgb, 1.0f);
}