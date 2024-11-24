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
Texture2D NormalTexture : register(t1);
Texture2D DepthTexture : register(t2);
Texture2D NoiseTexture : register(t3);