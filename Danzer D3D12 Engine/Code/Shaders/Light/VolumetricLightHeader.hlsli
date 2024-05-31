cbuffer Camera : register(b0)
{
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4   CameraPositon;
    
    float cameraTash[7];
}

cbuffer Shadow : register(b1)
{
    float4x4 LightMatrix;
    float4x4 LightProjection;
    float4   LightDirection;
    float4   LightColor;
    
    float4 lightTrashTwo[6];
}

cbuffer VolumetricLight : register(b2)
{
    uint   NUMBER_OF_STEPS;
    float  G_Scattering;
    float  ScatteringStrength;
    
    float  vlTrashOne;
    float4 vlTrashTwo[15];
}

SamplerState samplerState      : register(s0);

Texture2D WorldPositionTexture : register(t0);
Texture2D ShadowDepthTexture   : register(t1);
