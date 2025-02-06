struct InputToVertex
{
    float4 m_position : POSITION;
    float4 m_color    : COLOR;
    float  m_radius   : RADIUS;
};

struct VertexToPixel
{
    float4 m_svPosition    : SV_Position;
    float4 m_LightPosition : POSITION;
    float4 m_color         : COLOR;
    float2 m_uv            : UV;
    float  m_radius        : RADIUS;
};

cbuffer CameraBuffer : register(b0)
{
    float4x4 CameraProjection;
    float4x4 CameraTransform;
    float4   CameraPosition;
    
    float4 trash[7];
}