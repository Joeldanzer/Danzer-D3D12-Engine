#include "FullscreenHeader.hlsli"
#include "OLD_SSAOHeader.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 worldPos = PositionTexture.Sample(defaultSample, input.m_uv);
    if (!(length(worldPos.xyz) > 0))
        discard;
    
    float3 fragPos = mul(worldPos, CameraTransform).xyz;
    float3 normal = normalize(NormalTexture.Sample(defaultSample, input.m_uv).xyz);
    float3 noise = normalize(NoiseTexture.Sample(defaultSample, input.m_uv * NoiseScale));

    float3 tangent = normalize(noise - normal * dot(noise, normal));
    float3 biTangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, biTangent, normal);
    
    float radius = 1.0f;
    float bias = 0.005f;
    float occlusion = 0.0f;
    
    for (int i = 0; i < 64; i++)
    {
        float3 sample;
        if (i >= 32)
            sample = SamplesTwo[i - 32];
        else
            sample = SamplesOne[i];

        float3 samplePos = mul(sample, TBN);
        samplePos = fragPos.xyz + (samplePos) * radius;
        
        float4 offset = float4(samplePos, 1.0f);
        offset = mul(offset, CameraProjection);
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;
        offset.y = 1.0f - offset.y;
        
        float4 depthPosition = PositionTexture.Sample(defaultSample, offset.xy);
        float depth = mul(depthPosition, CameraTransform).z;
        
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - depth));
        occlusion += (depth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;
    }
    
    occlusion = 1.0f - (occlusion / 64);
     
    return float4(occlusion, occlusion, occlusion, 1.0f);
}