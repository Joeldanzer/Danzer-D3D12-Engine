//cbuffer MainCamera : register(b0)
//{
//    float4x4 CameraProjection;
//    float4x4 CameraTransform;
//    float4 CameraPosition;
//    float4 Eye;
//    
//    float Time;
//    float ScreenWidth;
//    float ScreenHeight;
//    
//    // Needs to be 256-byte alligned
//    float  trashOne;
//    float4 trashTwo[5];
//}
cbuffer KernelSamples : register(b1)
{
    float4 Samples[64];
}
cbuffer WindowScale : register(b2)
{
    uint   SampleCount;
    float2 NoiseScale;
    
    float  scaleTrashOne;
    float4 scaleTrashTwo[15];
    
}

SamplerState defaultSample : register(s0);

Texture2D PositionTexture : register(t0);
Texture2D NormalTexture   : register(t1);
Texture2D NoiseTexture    : register(t2);