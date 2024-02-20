cbuffer MainCamera : register(b0)
{
    float4x4 CameraProjection;
    float4x4 CameraTransform;
    float4 CameraPosition;
    float4 Eye;
    
    float Time;
    float ScreenWidth;
    float ScreenHeight;
    
    // Needs to be 256-byte alligned
    float  trashOne;
    float4 trashTwo[5];
}
cbuffer KernelSamplesOne : register(b1)
{
    float3 SamplesOne[32];
    float2 m_noiseScale;
    
    //float SampleTrash[62];
}
cbuffer KernelSamplesTwo : register(b2)
{
    float3 SamplesTwo[32];
    float2 SampleTwoTrash;
}

SamplerState defaultSample : register(s0);

Texture2D PositionTexture : register(t0);
Texture2D NormalTexture   : register(t1);
Texture2D DepthTexture    : register(t2);
Texture2D NoiseTexture    : register(t3);