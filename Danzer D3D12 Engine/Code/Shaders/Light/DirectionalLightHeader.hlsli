cbuffer DirectionalLight : register(b1)
{
    column_major float4x4 LightProjection;
    column_major float4x4 LightTransform;
    float4 LightColor;
    float4 AmbientColor;
    float4 LightDirection;
    
    float4 trashDirectionalLightBuffer[5];
};