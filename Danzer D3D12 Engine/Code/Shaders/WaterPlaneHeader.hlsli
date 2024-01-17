cbuffer WaterPlane : register(b1)
{
    float3 WaterColor;
    float Metallic;
    float Roughness;
    
    float2 WaterDirectionOne;
    float2 WaterDirectionTwo;
    float  Speed;
    
    float noiseScale;
    float heightScale;
}


Texture2D vertexNoise    : register(t0);
Texture2D noiseNormalOne : register(t1);
Texture2D noiseNormalTwo : register(t2);