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
    float textureScale;
}

Texture2D vertexNoise    : register(t1);
Texture2D noiseNormalOne : register(t2);
Texture2D noiseNormalTwo : register(t3);