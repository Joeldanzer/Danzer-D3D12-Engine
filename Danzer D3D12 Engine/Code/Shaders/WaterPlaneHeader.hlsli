cbuffer WaterPlane : register(b1)
{
    float3 WaterColorOne;
    float Metallic;
    float3 WaterColorTwo;
    float Roughness;
    
    float2 WaterDirectionOne;
    float2 WaterDirectionTwo;
    
    float Speed;
    float NoiseScale;
    float HeightScale;
    float TextureScale;
    
    float3 EdgeColor;
    float  Far;
    
    float  Near;
    float  EdgeScale; 
    float4 WaterPlaneTrash[10];
}

Texture2D vertexNoise    : register(t1);
Texture2D noiseNormalOne : register(t2);
Texture2D noiseNormalTwo : register(t3);