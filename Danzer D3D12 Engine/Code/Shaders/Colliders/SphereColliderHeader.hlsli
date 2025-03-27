struct Plane
{
    float3 m_normal;
    float  m_distance;
};

struct InputToVertex
{
    float4   m_position : POSITION;
    float4   m_color    : COLOR;
    float    m_radius   : RADIUS;
};

struct VertexToPixel
{
    float4 m_svPosition : SV_Position;
    float4 m_color      : COLOR;
    float4 m_position   : POSITION;
    float  m_radius     : RADIUS;
};

cbuffer CameraBuffer : register(b0){
    float4x4 Projection;
    float4x4 Camera;
    
    float4 trash[16];
}