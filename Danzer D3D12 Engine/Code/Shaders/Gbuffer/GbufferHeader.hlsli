struct InputToVertex
{
    float4   m_position    : POSITION;
    float4   m_normal      : NORMAL;
    float4   m_tangent     : TANGENT;
    float4   m_biNormal    : BINORMAL;
    float4   m_color       : COLOR;
    float2   m_uv          : UV;
    float4x4 m_transform : TRANSFORM;
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
    float4x4 CameraProjection;
    float4x4 CameraTransform;
    float4 CameraPosition;
    float4 Eye;

    // Needs to be 256-byte alligned
    float4 trash[6];
}

cbuffer Material : register(b1)
{
    float Metallic;
    float Roughness;
    float Emissive;
    float4 MaterialColor;
    
    //256-byte alligned
    float4 trashMaterial[14];
}

SamplerState defaultSampler   : register(s0);
Texture2D    albedoTexture    : register(t0);
Texture2D    normalTexture    : register(t1);
Texture2D    metallicTexture  : register(t2);
Texture2D    roughnessTexture : register(t3);
Texture2D    heightTexture    : register(t4);
Texture2D    aoTexrure        : register(t5);