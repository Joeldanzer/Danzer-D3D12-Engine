struct InputToVertex
{
    float4 m_color : COLOR;
    float4 m_start : POSITION;
    float4 m_end   : DESTINATION;
};

struct VertexToGeometry
{
    float4 m_color : COLOR;
    float4 m_start : POSITION;
    float4 m_end   : DESTINATION;
};