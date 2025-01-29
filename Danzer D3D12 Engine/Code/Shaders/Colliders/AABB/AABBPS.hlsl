#include "AABBStructs.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{	
	return float4(input.m_color.rgb, 1);
}