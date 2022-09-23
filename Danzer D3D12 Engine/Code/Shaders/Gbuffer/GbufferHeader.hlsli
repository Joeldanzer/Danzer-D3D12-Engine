struct InputToVertex
{
    float4                m_position    : POSITION;
    float4                m_normal      : NORMAL;
    float4                m_tangent     : TANGENT;
    float4                m_biNormal    : BINORMAL;
    float4                m_color       : COLOR;
    float2                m_uv          : UV;
    column_major float4x4 m_transform   : TRANSFORM;
};

struct VertexToPixel
{
    float4 m_position      : SV_Position;
    float4 m_normal        : NORMAL;
    float4 m_tangent       : TANGENT;
    float4 m_biNormal      : BINORMAL;
    float4 m_color         : COLOR;
    float4 m_worldPosition : WORLD_POSITION;
    float2 m_uv            : UV;
};

cbuffer MainCamera : register(b0)
{
    float4x4 CameraTransform;
    float4x4 CameraProjection;
    float4   CameraPosition;
    float4   Eye;

    // Needs to be 256-byte alligned
    float4 trash[6];
}

cbuffer Material : register(b1)
{
    float Metallic;
    float Roughness;
    float Emissive;
    uint  HasMaterialTexture;
    
    //256-byte alligned
    float4 trashMaterial[15];
}

SamplerState defaultSampler   : register(s0);
Texture2D    albedoTexture    : register(t0);
Texture2D    normalTexture    : register(t1);
Texture2D    matrerialTexture : register(t2);