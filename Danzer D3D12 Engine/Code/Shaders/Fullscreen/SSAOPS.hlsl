#include "FullscreenHeader.hlsli"
#include "SSAOHeader.hlsli"


float4 main(VertexToPixel input) : SV_TARGET
{	
	float3 fragPos = PositionTexture.Sample(defaultSample, input.m_uv).rgb;
    float3 normal  = NormalTexture.Sample(defaultSample, input.m_uv).rgb;
    float3 noise   = NoiseTexture.Sample(defaultSample, input.m_uv * m_noiseScale);

    float3   tangent = normalize(noise - normal * dot(noise, normal));
    float3   biTangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, biTangent, normal);
    
    float radius    = 0.5f;
    float bias      = 0.025f;
    float occlusion = 0.0f;
    
    for (int i = 0; i < 64; i++)
    {
        float3 sample;
        if(i >= 32)
            sample = SamplesTwo[i - 32];
        else
            sample = SamplesOne[i];
        
        float3 samplePos = mul(TBN, sample);
        samplePos = fragPos + samplePos * radius;
        
        float4 offset = float4(samplePos, 1.0f);
        offset = mul(offset, CameraProjection);
        offset.xyz /= offset.w;
        offset.xyz  = offset.xyz * 0.5f + 0.5f;
        
        float depth = DepthTexture.Sample(defaultSample, offset.xy).r;
        
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - depth));
        occlusion += (depth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;
    }
    
    occlusion = 1.0f - (occlusion / 64);
     
    return float4(occlusion, occlusion, occlusion, 1.0f);
}