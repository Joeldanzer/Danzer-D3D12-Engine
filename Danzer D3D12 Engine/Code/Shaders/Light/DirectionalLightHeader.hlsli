cbuffer DirectionalLight : register(b1)
{
    column_major float4x4 LightTransform;
    column_major float4x4 LightProjection;
    float4 LightColor;
    float4 AmbientColor;
    float4 LightDirection;
    uint2  WindowSize;
    float2 trashTwo;
    
    float4 trashDirectionalLightBuffer[4];
};
