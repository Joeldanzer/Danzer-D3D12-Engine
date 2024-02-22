#include "defaultStruct.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    //const float PI = 3.14159265359;
    //
    //float3 N = normalize(input.m_worldPosition.xyz);
    //float4 irradiance; 
    //
    //float3 up = float3(0.0f, 1.0f, 0.0f);
    //float3 right = normalize(cross(up, N));
    //up = normalize(cross(N, right));
    //
    //float sampleDelta = 0.1f;
    //float nrSamples = 0.0f;
    //
    //for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    //{ 
    //    for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
    //    {
    //        float3 tangetSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    //        float3 sampleVec = tangetSample.x * right + tangetSample.y * up + tangetSample.z * N;
    //        
    //        irradiance.rgb += skyboxTexture.Sample(defaultSampler, sampleVec) * cos(theta) * sin(theta);
    //        nrSamples++;
    //    }
    //}
    //
    //irradiance.rgb = PI * irradiance * (1.0f / nrSamples);
    //irradiance.a = 1.0f;
    float4 color = skyboxTexture.Sample(defaultSampler, input.m_worldPosition.xyz);
    
    
	return color;
}