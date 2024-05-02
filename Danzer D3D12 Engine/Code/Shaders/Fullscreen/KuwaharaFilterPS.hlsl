cbuffer FilterData : register(b0)
{
    uint2 WindowSize;
    uint  Radius;
    
    float trashOne;
    float4 trashTwo[15];
}

SamplerState filterSampler : register(s0);

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}