#include "ColliderStructs.hlsli"

float4 main(GeometryToPixel input) : SV_TARGET
{
    float4 color = input.m_color;
	return color;
}