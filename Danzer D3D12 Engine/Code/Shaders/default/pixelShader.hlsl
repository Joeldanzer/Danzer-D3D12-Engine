#include "defaultStruct.hlsli"

float4 main(VertexToPixel input) : SV_Target
{
    // Flips UV 
    float2 uv = input.m_uv;
    uv.x = 1 - uv.x;
    
    float4 color = albedo.Sample(defaultSampler, uv);
    float distanceVariable = min(1.f, distance(CameraPosition.xyz, input.m_worldPosition.xyz) / 150.f);
    
    //color = lerp(color, float4(0.5f, 0.5f, 1.f, color.a), distanceVariable);
    return color;
}