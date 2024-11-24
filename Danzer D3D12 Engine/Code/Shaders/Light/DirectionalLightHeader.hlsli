cbuffer DirectionalLight : register(b1)
{
    column_major float4x4 LightTransform;
    column_major float4x4 LightProjection;
    float4 LightDirection;
    float4 LightColor;
    float4 AmbientColor;
    uint2  WindowSize;
    float2 trashDirTwo;
    
    float4 trashDirectionalLightBuffer[4];
};
