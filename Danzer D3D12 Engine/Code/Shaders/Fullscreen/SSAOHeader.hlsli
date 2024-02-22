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
    float4 SamplesOne[32];
}
cbuffer KernelSamplesTwo : register(b2)
{
    float4 SamplesTwo[32];
}

cbuffer WindowScale : register(b3)
{
    float2 NoiseScale;
}

SamplerState defaultSample : register(s0);

Texture2D PositionTexture : register(t0);
Texture2D NormalTexture   : register(t1);
Texture2D DepthTexture    : register(t2);
Texture2D NoiseTexture    : register(t3);