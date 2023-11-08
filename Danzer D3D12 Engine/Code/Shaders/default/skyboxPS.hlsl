#include "defaultStruct.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = skyboxTexture.Sample(defaultSampler, input.m_worldPosition.xyz) + float4(0.2f, 0.2f, 0.2f, 0.f);
	return color;
}