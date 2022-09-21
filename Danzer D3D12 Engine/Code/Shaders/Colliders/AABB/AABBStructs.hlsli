struct InputToVertex
{
    float3 m_position : POSITION; 
    float3 m_size     : SIZE;
};

struct VertexToGeometry
{
    float4 m_position : SV_Position;
    float3 m_size     : SIZE;
};
