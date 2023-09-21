 
struct InputToVertex
{
    float4                m_position    : POSITION;
    float2                m_uv          : UV;
    column_major float4x4 m_transform   : TRANSFORM;
};

struct VertexToPixel
{
    float4 m_position      : SV_Position;
    float4 m_worldPosition : WORLD_POSITION;
    float2 m_uv            : UV;
};

cbuffer MainCamera : register(b0)
{
    float4x4 CameraTransform;
    float4x4 CameraProjection;
    float4   CameraPosition;
    
    // Needs to be 256-byte alligned
    float4 trash[7];
}

TextureCube skyboxTexture    : register(t0);
Texture2D albedo             : register(t1);
SamplerState defaultSampler  : register(s0);
//Ras defaultRasterizer : register(s1);