struct InputToVertex
{
    float3 m_position    : POSITION;
    float3 m_destination : DESTINATION;
};

struct VertexToGeometry
{
    float4 m_position    : POSITION;
    float4 m_destination : DESTINATION;
};