struct InputToVertex
{
    unsigned int m_vertexID : SV_VertexID;
};

struct VertexToPixel
{
    float4 m_position : SV_Position;
    float2 m_uv       : UV;
};