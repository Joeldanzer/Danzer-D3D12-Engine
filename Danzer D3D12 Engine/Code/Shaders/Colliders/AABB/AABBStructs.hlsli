struct InputToVertex
{
    float4   m_position  : POSITION;
    float4   m_color     : COLOR;
    float4x4 m_transform : TRANSFORM;
};

struct VertexToPixel
{
    float4 m_position : SV_Position;
    float4 m_color    : COLOR;
};

cbuffer CameraBuffer : register(b0)
{
    float4x4 projection;
    float4x4 camera;
    
    float4 m_trash[8];
}