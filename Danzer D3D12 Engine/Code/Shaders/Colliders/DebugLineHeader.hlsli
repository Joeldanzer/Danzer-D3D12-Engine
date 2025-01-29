struct InputToVertex
{
    float4 m_color : COLOR;
    float4 m_start : POSITION;
    float4 m_end   : END;
};

struct VertexToGeometry
{
    float4 m_color : COLOR;
    float4 m_start : POSITION;
    float4 m_end   : END;
};

struct GeometryToPixel
{
    float4 m_position : SV_Position;
    float4 m_color    : COLOR;
};

cbuffer CameraBuffer : register(b0)
{
    float4x4 projection;
    float4x4 view;
    
    float4 trash[8];
}