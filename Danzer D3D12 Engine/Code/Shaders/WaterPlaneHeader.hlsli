cbuffer WaterPlane : register(b1)
{
    float3 WaterColorOne;
    float3 WaterColorTwo;
    float Metallic;
    float Roughness;
    
    float2 WaterDirectionOne;
    float2 WaterDirectionTwo;
    float  Speed;
    
    float NoiseScale;
    float HeightScale;
    float TextureScale;
    
    float  Far;
    float  Near;
    float  EdgeScale;
    float3 EdgeColor;
}

Texture2D vertexNoise    : register(t1);
Texture2D noiseNormalOne : register(t2);
Texture2D noiseNormalTwo : register(t3);