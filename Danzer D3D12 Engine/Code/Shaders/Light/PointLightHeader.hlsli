cbuffer PointLightBuffer : register(b1){
    float4 LightColor;
    float  LightRange;
    float3 LightPosition;
    
    float4 LightTrash[14];
}