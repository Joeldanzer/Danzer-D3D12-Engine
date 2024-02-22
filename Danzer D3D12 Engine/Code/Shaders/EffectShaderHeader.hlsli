struct InputToVertex
{
    float4   m_position    : POSITION;
    float4   m_normal      : NORMAL;
    float4   m_tangent     : TANGENT;
    float4   m_biNormal    : BINORMAL;
    float4   m_color       : COLOR;
    float2   m_uv          : UV;
    float4x4 m_transform   : TRANSFORM;
};

struct VertexToPixel
{
    float4 m_position      : SV_Position;
    float4 m_normal        : NORMAL;
    float4 m_tangent       : TANGENT;
    float4 m_biNormal      : BINORMAL;
    float4 m_color         : COLOR;
    float4 m_worldPosition : WORLD_POSITION;
    float3 m_noiseWorldPos : NOISE_POSITION;
    float2 m_uv            : UV;
};

cbuffer MainCamera : register(b0)
{
    float4x4 CameraProjection;
    float4x4 CameraTransform;
    float4 CameraPosition;
    float4 Eye;
    
    float Time;
    float ScreenWidth;
    float ScreenHeight;
    
    // Needs to be 256-byte alligned
    float  trashOne;
    float4 trashTwo[5];
}

SamplerState effectSampler : register(s0);
Texture2D    depthTexture  : register(t0);

