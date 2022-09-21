cbuffer MainCamera : register(b0)
{
    float4x4 CameraTransform;
    float4x4 CameraProjection;
    float4   CameraPosition;
    float4 Eye;
    

    // Needs to be 256-byte alligned
    float4 trashCameraBuffer[6];
};

cbuffer DirectionalLight : register(b1)
{
    float4 LightColor;
    float4 AmbientColor;
    float4 LightDirection;
    
    float4 trashDirectionalLightBuffer[13];
};

SamplerState defaultSample     : register(s0);

Texture2D albedoTexture        : register(t0);
Texture2D normalTexture        : register(t1);
Texture2D materialTexture      : register(t2);
Texture2D vertexColorTexture   : register(t3);
Texture2D vertexNormalTexture  : register(t4);
Texture2D worldPositionTexture : register(t5);
TextureCube skyboxTexture      : register(t6);